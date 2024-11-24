#! /usr/bin/env python3

import json
import sys

def target_prefix(target):
  PREFIX_MAP = {
    'interface': 'sgpu',
    'metal': 'sgpu_mtl',
    'vulkan': 'sgpu_vk',
    'directx': 'sgpu_dx',
  }
  return PREFIX_MAP[target]

def generate_header(registry, target, file):
  file.write('/* generated from source/spargel/gpu/registry.json */\n')
  file.write('#pragma once\n')
  if target != 'interface':
    file.write('#include <spargel/gpu/gpu.h>\n')

def generate_ident(target, name):
  prefix = target_prefix(target)
  return f'{prefix}_{name}'

def generate_opaque(registry, target, file, item):
  impl_name = generate_ident(target, item['name'])
  opaque_name = f'{impl_name}_id'
  file.write(f'typedef struct {impl_name}* {opaque_name};\n')

def generate_enum_value(registry, target, file, item, val):
  enum_name = generate_ident(target, item['name'])
  name = f'{enum_name}_{val}'.upper()
  file.write(f'  {name},\n')

def generate_enum(registry, target, file, item):
  enum_name = generate_ident(target, item['name'])
  file.write(f'enum {enum_name} {{\n')
  for val in item['values']:
    generate_enum_value(registry, target, file, item, val)
  file.write(f'}};\n')

def translate_type(registry, target, name):
  name_parts = name.split(':')
  name_parts.reverse()
  base_name = name_parts[0]
  type_def = [x for x in registry['types'] if x['name'] == base_name][0]
  if type_def['kind'] == 'external':
    result = base_name
  elif type_def['kind'] == 'struct':
    result = 'struct ' + generate_ident('interface', base_name)
  elif type_def['kind'] == 'enum':
    result = 'int'
  else:
    result = generate_ident(target, base_name)
  if type_def['kind'] == 'opaque':
    result += '_id'
  for mod in name_parts[1:]:
    if mod == 'ref':
      result += '*'
    elif mod == 'cref':
      result += ' const*'
  return result

def generate_struct_fields(registry, target, file, item, field):
  field_name = field['name']
  type = translate_type(registry, target, field['type'])
  file.write(f'  {type} {field_name};\n')

def generate_struct(registry, target, file, item):
  struct_name = generate_ident(target, item['name'])
  file.write(f'struct {struct_name} {{\n')
  for field in item['fields']:
    generate_struct_fields(registry, target, file, item, field)
  file.write(f'}};\n')

def generate_type(registry, target, file, item):
  kind = item['kind']
  if kind == 'external':
    return
  elif kind == 'opaque':
    generate_opaque(registry, target, file, item)
  elif kind == 'enum' and target == 'interface':
    generate_enum(registry, target, file, item)
  elif kind == 'struct' and target == 'interface':
    generate_struct(registry, target, file, item)
  else:
    return

def generate_types(registry, target, file):
  types = registry['types']
  for item in types:
    generate_type(registry, target, file, item)

def generate_function(registry, target, file, item):
  func_name = generate_ident(target, item['name'])
  ret_type = translate_type(registry, target, item['return'])
  arg_list = []
  for arg in item['arguments']:
    arg_name = arg['name']
    arg_type = translate_type(registry, target, arg['type'])
    arg_list.append(f'{arg_type} {arg_name}')
  args = ', '.join(arg_list)
  file.write(f'{ret_type} {func_name}({args});\n')

def generate_functions(registry, target, file):
  functions = registry['functions']
  for item in functions:
    generate_function(registry, target, file, item)

def generate(registry, target, file):
  generate_header(registry, target, file)
  generate_types(registry, target, file)
  generate_functions(registry, target, file)

def generate_invoke_args(registry, target, file, item):
  arg_list = []
  for arg in item['arguments']:
    arg_name = arg['name']
    arg_type = translate_type(registry, 'interface', arg['type'])
    arg_target_type = translate_type(registry, target, arg['type'])
    if arg_type != arg_target_type:
      arg_list.append(f'({arg_target_type}){arg_name}')
    else:
      arg_list.append(f'{arg_name}')
  return ', '.join(arg_list)

def generate_func_impl(registry, file, item):
  func_name = generate_ident('interface', item['name'])
  ret_type = translate_type(registry, 'interface', item['return'])
  arg_list = []
  for arg in item['arguments']:
    arg_name = arg['name']
    arg_type = translate_type(registry, 'interface', arg['type'])
    arg_list.append(f'{arg_type} {arg_name}')
  args = ', '.join(arg_list)
  file.write(f'{ret_type} {func_name}({args}) {{\n')

  has_return = ret_type != "void"
  is_root = item['name'] == 'create_instance'

  dispatch_arg = item['arguments'][0]
  dispatch_name = dispatch_arg['name']
  if is_root:
    file.write(f'  switch (descriptor->backend) {{\n')
  else:
    file.write(f'  int backend = *(int*){dispatch_name};\n')
    file.write(f'  switch (backend) {{\n')

  file.write(f'#if SPARGEL_GPU_ENABLE_METAL\n')
  file.write(f'    case SGPU_BACKEND_METAL:\n')

  mtl_func_name = generate_ident('metal', item['name'])
  mtl_args = generate_invoke_args(registry, 'metal', file, item)
  if has_return:
    file.write(f'      return {mtl_func_name}({mtl_args});\n')
  else:
    file.write(f'      {mtl_func_name}({mtl_args});\n')
    file.write(f'      break;\n')

  file.write(f'#endif\n')

  file.write(f'#if SPARGEL_GPU_ENABLE_VULKAN\n')
  file.write(f'    case SGPU_BACKEND_VULKAN:\n')

  vk_func_name = generate_ident('vulkan', item['name'])
  vk_args = generate_invoke_args(registry, 'vulkan', file, item)
  if has_return:
    file.write(f'      return {vk_func_name}({vk_args});\n')
  else:
    file.write(f'      {vk_func_name}({vk_args});\n')
    file.write(f'      break;\n')

  file.write(f'#endif\n')

  file.write(f'#if SPARGEL_GPU_ENABLE_DIRECTX\n')
  file.write(f'    case SGPU_BACKEND_DIRECTX:\n')

  dx_func_name = generate_ident('directx', item['name'])
  dx_args = generate_invoke_args(registry, 'directx', file, item)
  if has_return:
    file.write(f'      return {dx_func_name}({dx_args});\n')
  else:
    file.write(f'      {dx_func_name}({dx_args});\n')
    file.write(f'      break;\n')

  file.write(f'#endif\n')

  file.write(f'    default:\n')
  file.write(f'      sbase_unreachable();\n')
  file.write(f'  }}\n')
  file.write(f'}}\n')

def generate_impl(registry, file):
  file.write(
    "#include <spargel/base/base.h>\n"
    "#include <spargel/gpu/gpu.h>\n\n"
    "#if SPARGEL_GPU_ENABLE_METAL\n"
    "#include <spargel/gpu/gpu_mtl.h>\n"
    "#endif\n\n"
    "#if SPARGEL_GPU_ENABLE_VULKAN\n"
    "#include <spargel/gpu/gpu_vk.h>\n"
    "#endif\n\n"
    "#if SPARGEL_GPU_ENABLE_DIRECTX\n"
    "#include <spargel/gpu/gpu_dx.h>\n"
    "#endif\n\n"
  )
  for item in registry['functions']:
    generate_func_impl(registry, file, item)

def main():
  with open('../source/spargel/gpu/registry.json') as f:
    registry = json.load(f)
  with open("../public/spargel/gpu/gpu.h", "w") as f:
    generate(registry, 'interface', f)
  with open("../source/spargel/gpu/gpu_mtl.h", "w") as f:
    generate(registry, 'metal', f)
  with open("../source/spargel/gpu/gpu_vk.h", "w") as f:
    generate(registry, 'vulkan', f)
  with open("../source/spargel/gpu/gpu_dx.h", "w") as f:
    generate(registry, 'directx', f)
  with open("../source/spargel/gpu/gpu.c", "w") as f:
    generate_impl(registry, f)
  return 0

if __name__ == '__main__':
  sys.exit(main())
