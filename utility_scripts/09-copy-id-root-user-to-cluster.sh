#!/bin/bash


set -e
set -x #echo on


ssh-copy-id 10.10.10.198
ssh-copy-id 10.10.10.201
ssh-copy-id 10.10.10.202
ssh-copy-id 10.10.10.203

