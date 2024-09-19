import subprocess

def run_command(command):
	"""Run a shell command and return output"""
	try:
		result = subprocess.run(command, shell=True, check=True, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		return result.stdout
	except subprocess.CalledProcessError as e:
		print("Error:", e.stderr)
	return None

def main():
	commands = ["make 2>error.log", 
		    "sudo insmod hello.ko",
		    "cat /proc/ethan_maze", 
		    "sudo rmmod hello"]

	# Run each command
	for command in commands:
		output = run_command(command)
		if output is None:
			print("Failed to run command",command)
		else:
			print(output)

if __name__ == "__main__":
	main()
