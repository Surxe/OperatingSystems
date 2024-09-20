import subprocess

def run_command(command, message):
    """Run a shell command and return the output, with a status message."""
    print(message)
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()

    if process.returncode != 0:
        print("Error: ",stderr.decode('utf-8', errors='replace').strip())
        return None
    return stdout.decode('utf-8', errors='replace').strip()

def main():
    # Define the commands and their associated messages
    commands = [
	{"cmd": "make", "msg": "Compiling the Makefile..."},
        {"cmd": "sudo insmod hello.ko", "msg": "Inserting the kernel module..."},
        {"cmd": "cat /proc/ethan_maze", "msg": "Reading the maze from /proc/ethan_maze..."},
        {"cmd": "sudo rmmod hello", "msg": "Removing the kernel module..."}
    ]

    for command in commands:
        output = run_command(command["cmd"], command["msg"])
        if command["cmd"] == "cat /proc/ethan_maze" and output:
            print(output)  # Print the maze output

if __name__ == "__main__":
    main()
