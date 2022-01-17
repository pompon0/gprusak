"""Embeds data files into a C module."""

def _cc_embed_data_impl(ctx):
  ctx.actions.run(
    inputs = ctx.files.srcs,
    outputs = [ctx.outputs.output_hdr,ctx.outputs.output_src],
    executable = ctx.executable.generator,
    arguments = [ctx.actions.args()
      .add_joined("--inputs",[l + ":" + f.files.to_list()[0].path for (f,l) in ctx.attr.srcs.items()],join_with=",")
      .add("--namespace",ctx.attr.namespace)
      .add("--output_hdr",ctx.outputs.output_hdr.path)
      .add("--output_src",ctx.outputs.output_src.path)],
  )

_cc_embed_data = rule(
  implementation = _cc_embed_data_impl,
  attrs = {
    "srcs": attr.label_keyed_string_dict(allow_files=True),
    "output_hdr": attr.output(),
    "output_src": attr.output(),
    "namespace": attr.string(),
    "generator": attr.label(
        executable = True,
        cfg = "host",
        default = Label("//gprusak:embed"),
    ),
  },
  # output_to_genfiles = True,
)

def cc_embed_data(
    name,
    srcs = dict(),
    namespace = "",
    **kwargs,
):
  output_hdr = name + ".h"
  output_src = name + ".cc"
  _cc_embed_data(
    name = name + "_gen",
    srcs = srcs,
    namespace = namespace,
    output_hdr = output_hdr,
    output_src = output_src,
  )

  native.cc_library(
    name = name,
    hdrs = [output_hdr],
    srcs = [output_src],
    **kwargs
  )
