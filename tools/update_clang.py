"""This script is used to download prebuilt clang binaries from chromium.
"""

import argparse
import os
import platform
import shutil
import sys
import tarfile
import tempfile
import urllib.request

# copied from
# https://chromium.googlesource.com/chromium/src/+/main/tools/clang/scripts/update.py
CLANG_REVISION = 'llvmorg-20-init-9764-gb81d8e90'
CLANG_SUB_REVISION = 6
PACKAGE_VERSION = '%s-%s' % (CLANG_REVISION, CLANG_SUB_REVISION)
CDS_URL = 'https://commondatastorage.googleapis.com/chromium-browser-clang'

THIS_DIR = os.path.abspath(os.path.dirname(__file__))
ROOT_DIR = os.path.abspath(os.path.join(THIS_DIR, '..'))
LLVM_BUILD_DIR = os.path.join(ROOT_DIR, 'third_party', 'llvm_build', 'cr')

STAMP_FILENAME = 'cr_build_revision'
STAMP_FILE = os.path.normpath(os.path.join(LLVM_BUILD_DIR, STAMP_FILENAME))

def RemoveTree(dir):
  shutil.rmtree(dir)

def ReadStampFile(path):
  """Return the contents of the stamp file, or '' if it doesn't exist."""
  try:
    with open(path, 'r') as f:
      return f.read().rstrip()
  except IOError:
    return ''

def EnsureDirExists(path):
  if not os.path.exists(path):
    os.makedirs(path)

def WriteStampFile(s, path):
  """Write s to the stamp file."""
  EnsureDirExists(os.path.dirname(path))
  with open(path, 'w') as f:
    f.write(s)
    f.write('\n')

def DownloadUrl(url, output_file):
  print(f'info: downloading {url} ')
  response = urllib.request.urlopen(url)
  output_file.write(response.read())

def DownloadAndUnpack(url):
  with tempfile.TemporaryFile() as f:
    DownloadUrl(url, f)
    f.seek(0)
    EnsureDirExists(LLVM_BUILD_DIR)
    t = tarfile.open(mode='r:*', fileobj=f)
    t.extractall(path=LLVM_BUILD_DIR)

def GetUrlPrefix(host_os):
  HOST_OS_URL_MAP = {
        'linux': 'Linux_x64',
        'mac': 'Mac',
        'mac-arm64': 'Mac_arm64',
        'win': 'Win',
    }
  return CDS_URL + '/' + HOST_OS_URL_MAP[host_os] + '/'

def DownloadAndUnpackPackage(package, host_os):
  cds_file = f'{package}-{PACKAGE_VERSION}.tar.xz'
  cds_full_url = GetUrlPrefix(host_os) + cds_file
  try:
    DownloadAndUnpack(cds_full_url)
  except:
    print('error: cannot download prebuilt clang package %s' % cds_file)
    sys.exit(1)

def GetHostOs():
  PLATFORM_HOST_OS_MAP = {
    'darwin': 'mac',
    'win32': 'win',
    'linux': 'linux',
  }
  host_os = PLATFORM_HOST_OS_MAP.get(sys.platform)
  if host_os == 'mac' and platform.machine() == 'arm64':
    host_os = 'mac-arm64'
  return host_os

def UpdatePackage(package, host_os):
  target_os = 'macos'
  expected_stamp = f'{PACKAGE_VERSION},{target_os}'
  if ReadStampFile(STAMP_FILE) == expected_stamp:
    return 0
  if package == 'clang' and os.path.exists(LLVM_BUILD_DIR):
    RemoveTree(LLVM_BUILD_DIR)
  DownloadAndUnpackPackage(package, host_os)
  WriteStampFile(expected_stamp, STAMP_FILE)
  return 0

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('--print-revision', action='store_true',
                      help='Print current clang revision and exit.')
  parser.add_argument('--package',
                      help='What package to update (default: clang)',
                      default='clang')
  parser.add_argument('--host-os',
                      help=('Which host OS to download for '
                            '(default: %(default)s)'),
                      default=GetHostOs(),
                      choices=('linux', 'mac', 'mac-arm64', 'win'))
  args = parser.parse_args()

  if args.print_revision:
    stamp_version = ReadStampFile(STAMP_FILE).partition(',')[0]
    print(stamp_version)
    if stamp_version != PACKAGE_VERSION:
      print(f'info: the expected clang version is {PACKAGE_VERSION}')
    return 0

  return UpdatePackage(args.package, args.host_os)

if __name__ == '__main__':
  sys.exit(main())
