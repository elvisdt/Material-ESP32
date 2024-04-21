idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "src" "libs/_AP_Server" "libs/_Wifi"
    EMBED_FILES
        "src/index.html"
        "src/ota.html"
        "src/restart.html"
        "src/favicon.png"
        "src/mini.css"
        "src/styles.css"
    #REQUIRES mdns
)
