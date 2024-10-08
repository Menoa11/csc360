Menoa Brennan README file with the rubric describing which reatures that I have implemented and if they work.


6. Rubric

Basic Functions (Total: 1 Mark, 0.25 Mark each)

Your submission contains a README.txt that clearly describes which features you have implemented and whether they work correctly or not. (Implemented and works)
Your submission can be successfully compiled by make without errors. The produced executable is named ssi. (Implemented and works)
Your ssi can be executed without additional parameters, showing a prompt waiting for user input. (Implemented and works)
The prompt is in the correct username@hostname: /home/user > format. (Implemented and works)


Foreground Execution (Total: 4 Marks, 1 Mark each)

Your ssi can execute ls without additional parameters and output the content correctly within the current directory. (Implemented and works)
Your ssi can execute ls with parameters, such as ls -alh, ls -alh ./, ls -l /usr/bin and output the content within the corresponding directory correctly. (Implemented and works)
Your ssi can execute long-running programs in the foreground, and it can respond to Ctrl-C to stop the program. For example, ping 1.1.1.1. (Implemented and works)
Your ssi can execute arbitrary external programs, and prints <name>: No such file or directory when the requested external program doesn't exist. (Implemented and works)


Changing Directories (Total: 5 Marks, 1 Mark each)

Your ssi can print the current working directory by pwd. (Implemented and works)
Your ssi can change to directories by specifying absolute directory paths, such as cd /usr/bin, cd /tmp. (Implemented and works)
Your ssi can handle relative directory paths, such as cd ., cd .., cd ../...
Your ssi can change to the user's home directory if no parameters are given to the cd command, and it can handle the special ~. (Implemented and works)
The command prompt always prints the current working directory correctly. (Implemented and works)


Background Execution (Total: 5 Marks, 1 Mark each)

Your ssi can use bg to execute programs in the background, such as bg cat foo.txt, bg ping 1.1.1.1. (Implemented and works)
When putting a job into the background using bg, your ssi can still accept user input in the foreground. (Implemented and works)
Your ssi can use bglist to list the currently running background jobs. (Implemented and works)
When a background job is finished, your ssi outputs <pid>: <program> <parameters> has terminated. when processing the next user input. (Implemented and works)
Your ssi can update background jobs correctly. For example, terminated background jobs should not appear in the bglist output. (Implemented and works)