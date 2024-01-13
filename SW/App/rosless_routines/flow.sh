#!/bin/bash

./waf configure

./waf build && ./build/rosless_routine ../../../ROS/arm_and_chassis_ws/src/common_teleop/routines/s3a/demo_nail_picker.moveitcmd #open_gripper.moveitcmd
