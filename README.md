Eclipse Artifacts
=================

`eclipse-artifacts` is a tool for Arch Linux which is a PKGBUILD generator for  eclipse plugins. You tell it the URL of the update site (the location where to download the plugin artifacts from) and the tool does the rest for you and creates a ready-to-use PKGBUILD.


When to use this tool?
---------------------

If the plugin developer offers a zip download with all the artifacts it might be easier to download that and write your PKGBUILD yourself.

If no zip file is provided but only atomic artifacts you could manually fetch all those by reading the site and feature XML files and assembling the download links from them. You would also need to calculate the md5/sha hashes yourself and finally write the PKGBUILD.

This doesn't make fun, especially for huge plugins. In this case you'd better use this tool.


How to get it?
--------------

Get it by cloning it with `git clone git://github.com/slopjong/eclipse-artifacts.git` or by the [direct download (v0.1)](https://github.com/slopjong/eclipse-artifacts/zipball/v0.1).

After cloning/extracting it go into the `eclipse-artifacts` directory (for the direct download it looks like `slopjong-eclipse-artifacts-5bdc503`). Then run `qmake` and `make`.

You should have got the binary eclipse-artifacts which is used as follows in your console:

```
$ ./eclipse-artifacts http://appwrench.onpositive.com/static/updatesite

Please provide some information about the eclipse plugin you're packaging.
The fields providing a default value can be left blank.

maintainer: Slopjong
email: slopjong@example.com
pkgname (add the eclipse- prefix) : eclipse-appwrench
pkgver: 1.3
pkgrel (default=1) :
description: Google App Engine Tools
url: http://example.com
license (default=custom) :
eclipsever (default=3.5) : 3.6
depends (list plugins only, space-seperated) : eclipse-gpe

```

On startup it will ask you for some package details.


What features are coming in the future?
---------------------------------------

Currently you have to execute `makepkg -s` or `makepkg -S` yourself after the PKGBUILD generation. Letting `eclipse-artifacts` do it is no big deal but it's not yet done. With a future version you should be able to create the source package and to upload it to the [Arch User Repository](http://aur.archlinux.org/) directly from this tool.

Some people prefer a graphical user interface. This will come too.

A dependency resolver would be nice to have too. In the current version you must type other required plugins.


Why does this tool not work with the update site I'm providing?
---------------------------------------------------------------

Until now only update sites are supported which provide a site.xml file. Update sites using the Equinox p2 mechanism don't work with this tool until now.
