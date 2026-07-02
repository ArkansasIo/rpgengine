# ArcLight Engine - Headless Server

It is a headless version of ArcLight Engine.

You can run it on headless servers that have neither X nor a graphics card.
This is ideal for running dedicated multiplayer game servers.


## How to use?

Instead of launching `arclight`, launch `arclight-headless` instead.

Since it has no interactive interface, you need to specify a script file, eg:

	./arclight-headless /abs/path/to/my/script.txt

You can create an appropriate script file using a compatible lobby client.
Usually, this will create a file `script.txt` for you in your writable data
directory, eg at `~/.arclight/script.txt`.

Once you have a `script.txt`, simply copy it to another name so that running
the lobby client again will not overwrite the old file. Now pass in the
absolute path to that file on the `arclight-headless` command-line.


## What is the license?

MIT, as for the rest of ArcLight Engine.
