#!/usr/bin/env bash

# This is the entrypoint script for the dockerfile. Executed in the
# container at runtime.

if [[ $# == 0 ]]; then
    cat /dockfpga/dockfpga
    exit 0
fi

# If we are running docker natively, we want to create a user in the container
# with the same UID and GID as the user on the host machine, so that any files
# created are owned by that user. Without this they are all owned by root.
# The dockfpga script sets the BUILDER_UID and BUILDER_GID vars.
if [[ -n $BUILDER_UID ]] && [[ -n $BUILDER_GID ]]; then
    groupadd -o -g $BUILDER_GID $BUILDER_GROUP 2> /dev/null
    useradd -o  -d $BUILDER_HOME -g $BUILDER_GID -u $BUILDER_UID $BUILDER_USER 2> /dev/null

    # Enable passwordless sudo capabilities for the user
    chown root:$BUILDER_GID $(which gosu)
    chmod +s $(which gosu); sync

    # Run the command as the specified user/group.
    exec gosu $BUILDER_UID:$BUILDER_GID "$@"
else
    # Just run the command as root.
    exec "$@"
fi
