# libRocket - The HTML / CSS User Interface Library

This is a version of **libRocket** that has been stripped of everything not
needed for **WNFramework**. The original **libRocket** can be found on [GitHub]
and we've included a copy of the [LICENSE](../LICENSE.librocket) file with this
repository. We expect to make heavy modifications and make backwards
incompatible changes to more easily integrate with the rest of **WNFramework**.
These changes will start small but grow as our needs evolve. After initial
bring-up of **libRocket** inside of **WNFramework**, we will be removing all
static global state from **libRocket**, with a goal of eventually removing all
non-smart-ptr memory allocations.

<!-- links -->
[GitHub]: https://github.com/libRocket/libRocket
