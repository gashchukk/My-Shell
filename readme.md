
# Lab work 8: myshell2
Authors (team): Authors (team): [Bohdan Hashchuk](https://github.com/gashchukk) <br> [Maximka Kutsenko](https://github.com/maxonchickdev)<br>
## Prerequisites
gcc/clang, Cmake, readline
### Compilation
```
chmod +x ./compile.sh
./compile.sh
```

### Usage
```
./myshell2
```
Built-in commands:
- mexit
- mcd
- mecho
- mpwd
- merrno
- mexport

But you can also run all the other possible commands in bash. Because, our program will create child process and run it in the bash<br>
Each of them support `-h | --help` flag to know what it does<br>
Also, you can run ```.msh```sripts using our myshell:
* `./ <script>` - to run it in the child process
* `. <script>` - to run it in this interpreter

### UPD. Support of pipelines and redirections
#### Redirection
Now you can redirect your output/ input in these ways:(similar to bash):
- `> <file_name>` - redirects STDOUT to file
- `2> <file_name>` - redirects STDERR to file
- `&> <file_name>` or `2>&1 <file_name>` - redirects both
- `< <file_name>` - read input from the file
#### Pipelines
In pipelines you can use both BuiltIn commands and external. Example:
- `ls | grep "abc" | wc -l` - it will outupt the number of lines in ls commands that have **abc**
- `ls | grep "abc" | wc -l > <file_name>` - will do the same but redirect output to the file

#### Run script in background
You can also run script in background
- `<script/command> &`

### Help tools
You can find `./clean.sh` script that cleanups all the execution attributes and .txt files that held rederected information


# My-Shell
