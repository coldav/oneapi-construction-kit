# Removing support for Vulkan

## Motivation for removal

Support for Vulkan is incomplete and cannot be used for full Vulkan support. It
is not being used anywhere at present and leads to requirement on installing the
Vulkan SDK in order to build OCK. This is also enabled by default and has led to
confusion among users of OCK, particularly in the requirements for building.

This also fits into an overall strategy of slimming OCK down to what is needed for
its purpose and reducing the overall technical debt and maintenance overheads.

## Proposal

Remove everything under source/vk and from all documentation that refers to it, as well
as any github workflow requirements such as installing the vulkan SDK.

## Alternative possibilities

We could keep it as is but have it disabled by default and only test it in our nightly testing. This also
Although
this has its merits, the author feels that Vulkan is of little benefit