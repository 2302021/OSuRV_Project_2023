
if not 'SIM' in globals():
	SIM = False
if not 'ARM' in globals():
	ARM = 's3a'

import subprocess
def run_for_stdout(str_cmd):
	r = subprocess.run(str_cmd.split(), stdout = subprocess.PIPE)
	return r.stdout.decode('utf-8')
on_rpi = run_for_stdout('lsb_release -i') == 'Distributor ID:	Raspbian\n'

arm_main = f'{ARM}_main'
if not on_rpi:
	if ARM == 's3a':
		arm_main = f'{ARM}_serial_main'

_settings = {
	'SIM': SIM,
	'ARM': ARM,
	'arm_main': arm_main,
}



_layout = [
	{
		'drv':
		# Tuple is vert pane split, while list is horiz.
		(
			['driver', 'app'],
			'dmesg',
		)
	},
]

build_drv = {
	'driver' : '''
		make
		''',
	'app' : '''
		./waf configure
		./waf build
		''',
}
# Key is target name, value is list of batches.
_targets = {
	# Done before everything.
	'setup': [
		{
			'driver': 'cd ../../SW/Driver/motor_ctrl/',
			'app': 'cd ../../SW/App/rosless_routines/',
			'dmesg': 'cd ../../SW/Driver/motor_ctrl/',
		}
		
	],
	'build_drv' : [
		build_drv
	],
	'__common_run_drv': [
		{
			'driver' : '''
				make stop
				make start
				''',
			'dmesg' : '''
				tmux_no_wait
				dmesg -w
				''',
		},
	],
	'run_drv': [
		{
			'app' : '''
				./build/rosless_routine ../../../ROS/arm_and_chassis_ws/src/common_teleop/routines/s3a/demo_nail_picker.moveitcmd
				''',
		},
	],



}

# Do not wait in last batch.
#last_batch = _targets['run'][-1]
#no_wait_last_batch = {}
#for pane, cmds in last_batch.items():
#	cmds = 'tmux_no_wait\n' + cmds
#	no_wait_last_batch[pane] = cmds
#_targets['run'][-1] = no_wait_last_batch

_dependencies = {
	'build_drv': ['setup'],
	'run_drv': ['setup', '__common_run_drv'],
}


