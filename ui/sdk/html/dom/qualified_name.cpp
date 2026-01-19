#include "html/dom/qualified_name.h"
#include "html/base/atomic_string.h"

namespace html {

const QualifiedName g_any_name(g_null_atom, g_null_atom, g_star_atom);
const QualifiedName g_null_name(g_null_atom, g_null_atom, g_null_atom);

}