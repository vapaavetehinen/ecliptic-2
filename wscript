#
# This file is the default set of rules to compile a Pebble application.
#
# Feel free to customize this to your needs.
#
import glob
import os
import os.path
import zipfile

top = '.'
out = 'build'


def options(ctx):
    ctx.load('pebble_sdk')


def configure(ctx):
    """
    This method is used to configure your build. ctx.load(`pebble_sdk`) automatically configures
    a build for each valid platform in `targetPlatforms`. Platform-specific configuration: add your
    change after calling ctx.load('pebble_sdk') and make sure to set the correct environment first.
    Universal configuration: add your change prior to calling ctx.load('pebble_sdk').
    """
    ctx.load('pebble_sdk')


def build(ctx):
    ctx.load('pebble_sdk')

    build_worker = os.path.exists('worker_src')
    binaries = []

    cached_env = ctx.env
    for platform in ctx.env.TARGET_PLATFORMS:
        ctx.env = ctx.all_envs[platform]
        ctx.set_group(ctx.env.PLATFORM_NAME)
        app_elf = '{}/pebble-app.elf'.format(ctx.env.BUILD_DIR)
        ctx.pbl_build(source=ctx.path.ant_glob('src/c/**/*.c'), target=app_elf, bin_type='app')

        if build_worker:
            worker_elf = '{}/pebble-worker.elf'.format(ctx.env.BUILD_DIR)
            binaries.append({'platform': platform, 'app_elf': app_elf, 'worker_elf': worker_elf})
            ctx.pbl_build(source=ctx.path.ant_glob('worker_src/c/**/*.c'),
                          target=worker_elf,
                          bin_type='worker')
        else:
            binaries.append({'platform': platform, 'app_elf': app_elf})
    ctx.env = cached_env

    ctx.set_group('bundle')
    ctx.pbl_bundle(binaries=binaries,
                   js=ctx.path.ant_glob(['src/pkjs/**/*.js',
                                         'src/pkjs/**/*.json',
                                         'src/common/**/*.js']),
                   js_entry_file='src/pkjs/index.js')
    ctx.add_post_fun(scrub_home_paths)


def scrub_home_paths(ctx):
    """
    The bundled JavaScript source map records absolute paths into the SDK, which
    live under the builder's home directory and so contain their username. Rewrite
    the finished .pbw with the home directory replaced by '~'.
    """
    home = os.path.expanduser('~').encode()
    if len(home) < 2:
        return
    for pbw in glob.glob(os.path.join(ctx.path.get_bld().abspath(), '*.pbw')):
        with zipfile.ZipFile(pbw) as src:
            entries = [(info, src.read(info.filename)) for info in src.infolist()]
        if not any(home in data for _, data in entries):
            continue
        tmp = pbw + '.tmp'
        with zipfile.ZipFile(tmp, 'w', zipfile.ZIP_DEFLATED) as dst:
            for info, data in entries:
                if info.filename.endswith(('.js', '.map', '.json')):
                    data = data.replace(home, b'~')
                dst.writestr(info, data)
        os.replace(tmp, pbw)
