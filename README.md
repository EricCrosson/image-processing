# FilterClient

What is FilterClient?

FilterClient is a silly name for a project that applies filters to
images. At this stage, the only filter implemented is an
edge-detecting Laplacian filter.

## How to get started

Browsing through the code is the best way to get a feel for this
project. It's short, and sweet. Dive right in to `mainwindow.cpp`!

## Compilation instructions

This project is quite simple, and thus follows the standard Qt
compilation procedure. Specifically, run the following commands:

```bash
qmake FilterClient.pro
make
```

## Beware

Of the hard-coded paths. I dropped this project quickly after the
initial proof-of-concept run; as such, it doesn't have many
bells-and-whistles that would qualify it for outsiders to
use. Instead, see the directory named `examples' for the kind of
output this program is capable of.

### Additional resources

I wrote a small bit about this project on my Wordpress blog. You can
find the article here:
[Image Processing in C++](http://ericscrosson.wordpress.com/2012/12/24/image-processing-in-cpp/)
