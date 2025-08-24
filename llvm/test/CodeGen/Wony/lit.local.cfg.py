config.suffixes = [".ll", ".mir"]

if not 'Wony' in config.root.targets:
    config.unsupported = True
