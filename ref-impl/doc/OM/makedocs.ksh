#
AUTODUCK="c:/program files/autoduck/autoduck.exe"
#
FLAGS="OMEXTERNAL | OMINTERNAL"
#
SRC="src/*.cpp src/*.h"
#
OUT=docs/ObjectManager.html
ADHTML="c:/program files/autoduck/html.fmt"
OMHTML="docs/html.fmt"
CONTENTS="docs/contents.d"
#
# html output
#
"${AUTODUCK}" /x "${FLAGS}" /s"${OMHTML}" /o ${OUT} /r html /f"${ADHTML}" ${SRC} "${CONTENTS}"
#
