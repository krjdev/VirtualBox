# CONTRIBUTING

## Important NOTE

Contributing to this repository is allowed, but I'm **currently** will not backport these changes to
the official **VirtualBox** SVN repository.

## GIT commit

For commit messages with **GIT** please use the following form.

### TITLE

```
[DIR]: [SUBDIR]: Short title
```

If the change is in the root directory of this repository don't add a directory in the title.

### DESCRIPTION

```
[DESCRIPTION]

The detailed description of the commit.
```

If it's a new file you can write **Nothing.** in the description.


### FILE(s)
```
[FILE]
A  path/to/file.c
```
Add the new, modified or renamed files here. Generally I would suggest to make a commit for a **single** file only. Use the output from **GIT** after a ``git add`` for the text in the commit message.

You can get the text, when you type the follwing in the command line:  
```
$ git status --short  
A  .github/CONTRIBUTING.md
$ 
```

### SIGNED-OFF

Use the follwing additional argument to get the **required** field in the final message.

```
$ git commit -s
$ 
```

### EXAMPLE (FULL)
```
.github: Add initial CONTRIBUTING.md

[DESCRIPTION]

Nothing.

[FILE]
A  .github/CONTRIBUTING.md

Signed-off-by: Johannes Krottmayer <krjdev@gmail.com>
```
