#
ADROOT="c:/program files/autoduck"
AUTODUCK="${ADROOT}/autoduck.exe"
#
FLAGS="OMEXTERNAL | OMINTERNAL"
#
SRC="src/*.cpp src/*.h"
#
OUT=docs/ObjectManager.html
ADHTML="${ADROOT}/html.fmt"
OMHTML="docs/html.fmt"
OMCONT="docs/contents.d"
#
"${AUTODUCK}" /x "${FLAGS}" /s"${OMHTML}" /o ${OUT} /r html /f"${ADHTML}" ${SRC} "${OMCONT}"
#
