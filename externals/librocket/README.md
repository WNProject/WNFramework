# libRocket - The HTML / CSS User Interface Library

This is a verison of libRocket that has been stripped of everything not needed
for WNFramework. The original libRocket can be found on [GitHub][GitHub] and
we've included a copy of the [LICENSE](LICENSE) file with this repository. We
expect to make heavy modifications and make backwards incompatible changes to
more easily integrate with the rest of WNFramework. These changes will start
small but grow as our needs evolve. After initial bring-up of librocket inside
of WNFramework, we will be removing all static global state from librocket, with
a goal of eventually removing all non-smart-ptr memory allocations.

<!-- links -->
[GitHub]: https://github.com/libRocket/libRocket
