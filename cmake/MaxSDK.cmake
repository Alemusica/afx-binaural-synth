
# Minimal Max SDK integration for CMake
if(NOT EXISTS "${MAX_SDK_DIR}")
  message(FATAL_ERROR "MAX_SDK_DIR non esiste: ${MAX_SDK_DIR}")
endif()

set(MAX_SDK_INCLUDE_DIRS
  "${MAX_SDK_DIR}/source/c74support/max-includes"
  "${MAX_SDK_DIR}/source/c74support/msp-includes"
)

# Max non richiede link di librerie statiche per gli externals; serve l'SDK per headers
set(MAX_SDK_LIBS "")
