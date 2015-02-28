conman
===
Configuration management binaries

## Synopsis
conman provides configuration management in the form of individual binaries. It provides a single binary per resource and enables more repeatable configuration management with shell scripts.

## Install
conman depends on [libconman](https://github.com/virginops/libconman).

	git clone git@github.com:virginops/libconman.git
	cd libconman
	make
	make install

Once libconman is installed:

	git clone git@github.com:virginops/conman.git
	cd conman
	make
	make install

## Examples
This example creates a user, the user's group and home directory, and two empty files in that user's home directory:

	cm-group -v foo
	cm-user -v -g foo -d /home/foo -s /bin/bash foo
	cm-dir -v -o foo -g foo -m 0700 /home/foo
	cm-file -v -o foo -g foo /home/foo/one
	cm-file -v -o foo -g foo /home/foo/two
