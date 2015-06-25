#pragma once

#include "field.hpp"

#include <string>

// Parsing the input. This assumes valid input to some degree (since that's guaranteed), so do not call it with invalid strings or correct output is obviously no longer possible.
Field parse( const std::string& input );
