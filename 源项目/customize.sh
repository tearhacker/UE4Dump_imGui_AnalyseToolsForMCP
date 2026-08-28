#!/system/bin/sh

# Keep configuration outside the replaceable module directory. The Zygisk
# companion reads these files as root for KernelSU/Zygisk Next compatibility.
PERSIST_DIR="/data/adb/zygisk_il2cpp_mcp"
LEGACY_DIR="/data/adb/modules/zygisk_il2cpp_mcp/config"
LEGACY_JAVA_HOOK_DIR="/data/adb/modules/zygisk_java_hook/config"

ui_print "- Initializing persistent MCP bridge config..."
mkdir -p "$PERSIST_DIR"

# Migrate config written by earlier versions, but never overwrite persistent
# user settings during an update.
if [ ! -f "$PERSIST_DIR/apps.txt" ]; then
    if [ -f "$MODPATH/config/apps.txt" ]; then
        cp -f "$MODPATH/config/apps.txt" "$PERSIST_DIR/apps.txt"
        ui_print "  Migrated apps.txt from module staging directory"
    elif [ -f "$LEGACY_DIR/apps.txt" ]; then
        cp -f "$LEGACY_DIR/apps.txt" "$PERSIST_DIR/apps.txt"
        ui_print "  Migrated apps.txt from legacy module directory"
    elif [ -f "$LEGACY_JAVA_HOOK_DIR/apps.txt" ]; then
        cp -f "$LEGACY_JAVA_HOOK_DIR/apps.txt" "$PERSIST_DIR/apps.txt"
        ui_print "  Migrated apps.txt from zygisk_java_hook"
    else
        echo "com.pi.czrxdfirst" > "$PERSIST_DIR/apps.txt"
        ui_print "  Created apps.txt (default: com.pi.czrxdfirst)"
    fi
fi

if [ ! -f "$PERSIST_DIR/port.txt" ]; then
    if [ -f "$MODPATH/config/port.txt" ]; then
        cp -f "$MODPATH/config/port.txt" "$PERSIST_DIR/port.txt"
        ui_print "  Migrated port.txt from module staging directory"
    elif [ -f "$LEGACY_DIR/port.txt" ]; then
        cp -f "$LEGACY_DIR/port.txt" "$PERSIST_DIR/port.txt"
        ui_print "  Migrated port.txt from legacy module directory"
    elif [ -f "$LEGACY_JAVA_HOOK_DIR/port.txt" ]; then
        cp -f "$LEGACY_JAVA_HOOK_DIR/port.txt" "$PERSIST_DIR/port.txt"
        ui_print "  Migrated port.txt from zygisk_java_hook"
    else
        echo "27184" > "$PERSIST_DIR/port.txt"
        ui_print "  Created port.txt (default: 27184)"
    fi
fi

if [ ! -f "$PERSIST_DIR/memory_backend.txt" ]; then
    echo "system" > "$PERSIST_DIR/memory_backend.txt"
    ui_print "  Created memory_backend.txt (default: system)"
fi

if [ ! -f "$PERSIST_DIR/driver_node.txt" ]; then
    : > "$PERSIST_DIR/driver_node.txt"
    ui_print "  Created driver_node.txt (automatic device discovery)"
fi

set_perm_recursive "$PERSIST_DIR" root root 0755 0644

ui_print "- Config ready: $PERSIST_DIR"
