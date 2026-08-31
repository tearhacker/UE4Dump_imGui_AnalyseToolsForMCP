#pragma once

#include <string>

#include <nlohmann/json.hpp>

namespace UmtMcp::Analysis
{
nlohmann::json InspectArtifactFile(const std::string &path, const std::string &source);
}
