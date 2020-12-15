//
// Created by Alex on 12/14/2020.
//

#pragma once

#include <string>
#include <vector>

namespace RPG::Vulkan {
	std::vector<std::string> GetRequiredVulkanExtensionNames();
	bool IsVulkanAvailable();
}


