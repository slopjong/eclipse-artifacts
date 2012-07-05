Eclipse Artifacts
=================

eclipse-artifacts is going to be an Arch Linux tool to generate a PKGBUILD for eclipse plugins. Currently it's under heavy development yet.

In other words, it's still a piece of junk which doesn't do anything useful right now.

How it will work
----------------

Pass the command line tool the eclipse update site and let it download for you all the plugin artifacts and calculating the md5 or sha hashes. The output will be a ready-to-use PKGBUILD so that you can immediately create the source package which you can upload in the [Arch User Repository](http://aur.archlinux.org/).