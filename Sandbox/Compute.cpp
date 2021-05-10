#include "Compute.h"

#include <vector>
#include <optional>
#include <set>
#include <fstream>
#include <vulkan\vulkan.h>

struct Instance
{
	VkInstance m_instance = VK_NULL_HANDLE;
};

Handle create_compute_instance()
{
	VkApplicationInfo application_info = {};
	application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	application_info.apiVersion = VK_API_VERSION_1_2;
	application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	application_info.pApplicationName = "Compute";
	application_info.pEngineName = "Compute";

	VkInstanceCreateInfo instance_create_info = {};
	instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.pNext = VK_NULL_HANDLE;
	instance_create_info.flags = VK_NULL_HANDLE;
	instance_create_info.pApplicationInfo = &application_info;
	instance_create_info.enabledLayerCount = 0;
	instance_create_info.ppEnabledLayerNames = VK_NULL_HANDLE;
	instance_create_info.enabledExtensionCount = 0;
	instance_create_info.ppEnabledExtensionNames = VK_NULL_HANDLE;

	Instance* p_instance = new Instance{};
	vkCreateInstance(&instance_create_info, nullptr, &p_instance->m_instance);

	return p_instance;
}

void destroy_compute_instance(Handle instance)
{
	Instance* p_instance = static_cast<Instance*>(instance);
	vkDestroyInstance(p_instance->m_instance, nullptr);

	delete p_instance;
}

struct Queue
{
	std::optional<uint32_t> m_compute_family = {};
	std::optional<uint32_t> m_transfer_family = {};

	VkQueue m_compute_queue = VK_NULL_HANDLE;
	VkQueue m_transfer_queue = VK_NULL_HANDLE;
};

struct Device
{
	Queue m_queue = {};

	VkDevice m_logical_device = VK_NULL_HANDLE;
	VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
};

bool check_physical_device_support(VkPhysicalDevice device)
{
	return true;
}

VkPhysicalDevice find_best_physical_device(Instance* p_instance)
{
	uint32_t physical_device_count = 0;
	vkEnumeratePhysicalDevices(p_instance->m_instance, &physical_device_count, nullptr);

	if (physical_device_count == 0)
		return nullptr;

	std::vector<VkPhysicalDevice> v_physical_devices(physical_device_count);
	vkEnumeratePhysicalDevices(p_instance->m_instance, &physical_device_count, v_physical_devices.data());

	Device* p_device = new Device{};
	VkPhysicalDeviceProperties v_physical_device_properties = {};
	for (auto physical_device : v_physical_devices)
		if (check_physical_device_support(physical_device))
			return physical_device;

	return VK_NULL_HANDLE;
}

bool is_queue_complete(const Queue& queue)
{
	return queue.m_compute_family.has_value() && queue.m_transfer_family.has_value();
}

Queue create_queue(VkPhysicalDevice physical_device)
{
	Queue queue = {};

	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

	std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

	int32_t i = 0;
	for (auto itr = queue_families.begin(); itr != queue_families.end(); itr++, i++)
	{
		if (itr->queueCount > 0)
		{
			if (itr->queueFlags & VK_QUEUE_COMPUTE_BIT)
				queue.m_compute_family = i;

			if (itr->queueFlags & VK_QUEUE_TRANSFER_BIT)
				queue.m_transfer_family = i;

			if (is_queue_complete(queue))
				break;
		}
	}

	return queue;
}

VkDevice create_logical_device(Instance* p_instance, VkPhysicalDevice physical_device, Queue queue)
{
	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	std::set<uint32_t> unique_queue_families = {
		queue.m_compute_family.value(),
		queue.m_transfer_family.value()
	};

	float queuePriority = 1.0f;
	for (uint32_t queue_family : unique_queue_families)
	{
		VkDeviceQueueCreateInfo queue_create_info = {};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = queue_family;
		queue_create_info.queueCount = 1;
		queue_create_info.pQueuePriorities = &queuePriority;
		queue_create_infos.push_back(queue_create_info);
	}

	// Physical device features.
	VkPhysicalDeviceFeatures device_features = {};
	device_features.samplerAnisotropy = VK_TRUE;
	device_features.sampleRateShading = VK_TRUE; // Enable sample shading feature for the device

	// Device create info.
	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.pEnabledFeatures = &device_features;
	create_info.enabledExtensionCount = 0;
	create_info.ppEnabledExtensionNames = VK_NULL_HANDLE;
	create_info.enabledLayerCount = 0;
	create_info.ppEnabledLayerNames = VK_NULL_HANDLE;

	// Create the logical device.
	VkDevice logical_device = VK_NULL_HANDLE;
	vkCreateDevice(physical_device, &create_info, nullptr, &logical_device);

	return logical_device;
}

Handle create_compute_device(Handle instance)
{
	Instance* p_instance = static_cast<Instance*>(instance);
	Device* p_device = new Device{};
	p_device->m_physical_device = find_best_physical_device(p_instance);

	if (p_device->m_physical_device == VK_NULL_HANDLE)
	{
		delete p_device;
		return nullptr;
	}

	p_device->m_queue = create_queue(p_device->m_physical_device);
	p_device->m_logical_device = create_logical_device(p_instance, p_device->m_physical_device, p_device->m_queue);

	vkGetDeviceQueue(p_device->m_logical_device, p_device->m_queue.m_compute_family.value(), 0, &p_device->m_queue.m_compute_queue);
	vkGetDeviceQueue(p_device->m_logical_device, p_device->m_queue.m_transfer_family.value(), 0, &p_device->m_queue.m_transfer_queue);

	return p_device;
}

void destroy_compute_device(Handle device)
{
	Device* p_device = static_cast<Device*>(device);
	vkDestroyDevice(p_device->m_logical_device, nullptr);

	delete p_device;
}

struct Pipeline
{
	VkPipeline m_pipeline = VK_NULL_HANDLE;
	VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
};

VkPipelineLayout create_pipeline_layout(VkDevice logical_device)
{
	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.pNext = VK_NULL_HANDLE;
	pipeline_layout_create_info.flags = VK_NULL_HANDLE;
	pipeline_layout_create_info.pushConstantRangeCount = 0;
	pipeline_layout_create_info.pPushConstantRanges = VK_NULL_HANDLE;
	pipeline_layout_create_info.setLayoutCount = 0;
	pipeline_layout_create_info.pSetLayouts = VK_NULL_HANDLE;

	VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
	vkCreatePipelineLayout(logical_device, &pipeline_layout_create_info, nullptr, &pipeline_layout);

	return pipeline_layout;
}

VkShaderModule create_shader_module(VkDevice logical_device, const char* shader_file)
{
	std::fstream shader_input(shader_file, std::ios::binary | std::ios::ate);
	uint64_t shader_size = shader_input.tellg();
	shader_input.seekg(0);

	std::vector<uint32_t> shader_code(shader_size);
	shader_input.read(reinterpret_cast<char*>(shader_code.data()), shader_size);
	shader_input.close();

	VkShaderModuleCreateInfo shader_module_create_info = {};
	shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_module_create_info.pNext = VK_NULL_HANDLE;
	shader_module_create_info.flags = VK_NULL_HANDLE;
	shader_module_create_info.codeSize = shader_size;
	shader_module_create_info.pCode = shader_code.data();

	VkShaderModule shader_module = VK_NULL_HANDLE;
	vkCreateShaderModule(logical_device, &shader_module_create_info, nullptr, &shader_module);

	return shader_module;
}

VkPipeline create_pipeline(VkDevice logical_device, VkPipelineLayout pipeline_layout, const char* shader_file)
{
	VkPipelineShaderStageCreateInfo shader_stage_create_info = {};

	VkComputePipelineCreateInfo compute_pipline_create_info = {};
	compute_pipline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	compute_pipline_create_info.pNext = VK_NULL_HANDLE;
	compute_pipline_create_info.flags = VK_NULL_HANDLE;
	compute_pipline_create_info.basePipelineHandle = VK_NULL_HANDLE;
	compute_pipline_create_info.basePipelineIndex = 0;
	compute_pipline_create_info.layout = pipeline_layout;
	compute_pipline_create_info.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	compute_pipline_create_info.stage.pNext = VK_NULL_HANDLE;
	compute_pipline_create_info.stage.flags = VK_NULL_HANDLE;
	compute_pipline_create_info.stage.pSpecializationInfo = VK_NULL_HANDLE;
	compute_pipline_create_info.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	compute_pipline_create_info.stage.pName = "main";
	compute_pipline_create_info.stage.module = create_shader_module(logical_device, shader_file);

	VkPipeline pipeline = VK_NULL_HANDLE;
	vkCreateComputePipelines(logical_device, VK_NULL_HANDLE, 1, &compute_pipline_create_info, nullptr, &pipeline);

	vkDestroyShaderModule(logical_device, compute_pipline_create_info.stage.module, nullptr);
	return pipeline;
}

Handle create_compute_pipeline(Handle device, const char* shader_file)
{
	Device* p_device = static_cast<Device*>(device);

	Pipeline* p_pipeline = new Pipeline{};
	p_pipeline->m_pipeline_layout = create_pipeline_layout(p_device->m_logical_device);
	p_pipeline->m_pipeline = create_pipeline(p_device->m_logical_device, p_pipeline->m_pipeline_layout, shader_file);

	return p_pipeline;
}

void destroy_compute_pipeline(Handle device, Handle pipeline)
{
	Device* p_device = static_cast<Device*>(device);
	Pipeline* p_pipeline = static_cast<Pipeline*>(pipeline);

	vkDestroyPipeline(p_device->m_logical_device, p_pipeline->m_pipeline, nullptr);
	vkDestroyPipelineLayout(p_device->m_logical_device, p_pipeline->m_pipeline_layout, nullptr);

	delete p_pipeline;
}

struct Buffer
{
	VkBuffer m_buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_buffer_memory = VK_NULL_HANDLE;
};

VkBuffer create_buffer(Device* device, uint64_t size)
{
	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.pNext = VK_NULL_HANDLE;
	buffer_create_info.flags = VK_NULL_HANDLE;
	buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	buffer_create_info.size = size;
	buffer_create_info.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	buffer_create_info.queueFamilyIndexCount = 0;
	buffer_create_info.pQueueFamilyIndices = VK_NULL_HANDLE;

	VkBuffer buffer = VK_NULL_HANDLE;
	vkCreateBuffer(device->m_logical_device, &buffer_create_info, nullptr, &buffer);

	return buffer;
}

VkDeviceMemory create_buffer_memory(Device* device, VkBuffer buffer)
{
	VkMemoryRequirements memory_requirements = {};
	vkGetBufferMemoryRequirements(device->m_logical_device, buffer, &memory_requirements);

	VkPhysicalDeviceMemoryProperties physical_device_memory_properties = {};
	vkGetPhysicalDeviceMemoryProperties(device->m_physical_device, &physical_device_memory_properties);

	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = memory_requirements.size;

	VkMemoryPropertyFlags memory_property_flags = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount; i++)
	{
		if ((memory_requirements.memoryTypeBits & (1 << i)) && (physical_device_memory_properties.memoryTypes[i].propertyFlags & memory_property_flags) == memory_property_flags)
		{
			memory_allocate_info.memoryTypeIndex = i;
			break;
		}
	}

	VkDeviceMemory device_memory = VK_NULL_HANDLE;
	vkAllocateMemory(device->m_logical_device, &memory_allocate_info, nullptr, &device_memory);
	vkBindBufferMemory(device->m_logical_device, buffer, device_memory, 0);

	return device_memory;
}

Handle create_compute_storage_buffer(Handle device, uint64_t size)
{
	Device* p_device = static_cast<Device*>(device);

	Buffer* p_buffer = new Buffer{};
	p_buffer->m_buffer = create_buffer(p_device, size);
	p_buffer->m_buffer_memory = create_buffer_memory(p_device, p_buffer->m_buffer);

	return p_buffer;
}

void* map_memory_from_buffer(Handle device, Handle buffer, uint64_t size, uint64_t offset)
{
	Device* p_device = static_cast<Device*>(device);
	Buffer* p_buffer = static_cast<Buffer*>(buffer);

	void* p_data_pointer = nullptr;
	vkMapMemory(p_device->m_logical_device, p_buffer->m_buffer_memory, offset, size, 0, &p_data_pointer);

	return p_data_pointer;
}

void unmap_memory_from_buffer(Handle device, Handle buffer)
{
	Device* p_device = static_cast<Device*>(device);
	Buffer* p_buffer = static_cast<Buffer*>(buffer);

	vkUnmapMemory(p_device->m_logical_device, p_buffer->m_buffer_memory);
}

void destroy_compute_storage_buffer(Handle device, Handle buffer)
{
	Device* p_device = static_cast<Device*>(device);
	Buffer* p_buffer = static_cast<Buffer*>(buffer);

	vkDestroyBuffer(p_device->m_logical_device, p_buffer->m_buffer, nullptr);
	vkFreeMemory(p_device->m_logical_device, p_buffer->m_buffer_memory, nullptr);

	delete p_buffer;
}

struct Image
{
	VkImage m_image = VK_NULL_HANDLE;
	VkDeviceMemory m_image_memory = VK_NULL_HANDLE;
};

VkImage create_image(Device* device, uint32_t width, uint32_t height, uint32_t depth)
{
	VkImageCreateInfo image_create_info = {};
	image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_create_info.pNext = VK_NULL_HANDLE;
	image_create_info.flags = VK_NULL_HANDLE;
	image_create_info.imageType = VK_IMAGE_TYPE_2D;
	image_create_info.usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
	image_create_info.queueFamilyIndexCount = 0;
	image_create_info.pQueueFamilyIndices = VK_NULL_HANDLE;
	image_create_info.extent.width = width;
	image_create_info.extent.height = height;
	image_create_info.extent.depth = depth;
	image_create_info.arrayLayers = 1;
	image_create_info.mipLevels = 1;

	VkImage image = VK_NULL_HANDLE;
	vkCreateImage(device->m_logical_device, &image_create_info, nullptr, &image);

	return image;
}

VkDeviceMemory create_image_memory(Device* device, VkImage image)
{
	VkMemoryRequirements memory_requirements = {};
	vkGetImageMemoryRequirements(device->m_logical_device, image, &memory_requirements);

	VkPhysicalDeviceMemoryProperties physical_device_memory_properties = {};
	vkGetPhysicalDeviceMemoryProperties(device->m_physical_device, &physical_device_memory_properties);

	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = memory_requirements.size;

	VkMemoryPropertyFlags memory_property_flags = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount; i++)
	{
		if ((memory_requirements.memoryTypeBits & (1 << i)) && (physical_device_memory_properties.memoryTypes[i].propertyFlags & memory_property_flags) == memory_property_flags)
		{
			memory_allocate_info.memoryTypeIndex = i;
			break;
		}
	}

	VkDeviceMemory device_memory = VK_NULL_HANDLE;
	vkAllocateMemory(device->m_logical_device, &memory_allocate_info, nullptr, &device_memory);
	vkBindImageMemory(device->m_logical_device, image, device_memory, 0);

	return device_memory;
}

Handle create_compute_storage_image(Handle device, uint32_t width, uint32_t height, uint32_t depth)
{
	Device* p_device = static_cast<Device*>(device);

	Image* image = new Image{};
	image->m_image = create_image(p_device, width, height, depth);
	image->m_image_memory = create_image_memory(p_device, image->m_image);

	return image;
}

void destroy_compute_storage_image(Handle device, Handle image)
{
	Device* p_device = static_cast<Device*>(device);
	Image* p_image = static_cast<Image*>(image);

	vkDestroyImage(p_device->m_logical_device, p_image->m_image, nullptr);
	vkFreeMemory(p_device->m_logical_device, p_image->m_image_memory, nullptr);

	delete p_image;
}

struct CommandBuffer
{
	VkCommandPool m_command_pool = VK_NULL_HANDLE;
	VkCommandBuffer m_command_buffer = VK_NULL_HANDLE;
};

VkCommandPool create_command_pool(Device* device)
{
	VkCommandPoolCreateInfo command_pool_create_info = {};
	command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_create_info.pNext = VK_NULL_HANDLE;
	command_pool_create_info.flags = VK_NULL_HANDLE;
	command_pool_create_info.queueFamilyIndex = device->m_queue.m_compute_family.value();

	VkCommandPool command_pool = VK_NULL_HANDLE;
	vkCreateCommandPool(device->m_logical_device, &command_pool_create_info, nullptr, &command_pool);

	return command_pool;
}

VkCommandBuffer create_command_buffer(Device* device, VkCommandPool command_pool)
{
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.pNext = VK_NULL_HANDLE;
	command_buffer_allocate_info.commandPool = command_pool;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer = VK_NULL_HANDLE;
	vkAllocateCommandBuffers(device->m_logical_device, &command_buffer_allocate_info, &command_buffer);

	return command_buffer;
}

void execute_compute_command(Handle device, Handle pipeline, uint64_t input_buffer_count, Handle* input_buffers, uint64_t output_buffer_count, Handle* output_buffers)
{
	Device* p_device = static_cast<Device*>(device);
	Pipeline* p_pipeline = static_cast<Pipeline*>(pipeline);

	std::vector<Buffer*> p_input_buffers;
	for (uint64_t count = 0; count < input_buffer_count; count++)
		p_input_buffers.push_back(static_cast<Buffer*>(input_buffers[count]));

	std::vector<Buffer*> p_output_buffers;
	for (uint64_t count = 0; count < output_buffer_count; count++)
		p_input_buffers.push_back(static_cast<Buffer*>(output_buffers[count]));

	CommandBuffer command_buffer = {};
	command_buffer.m_command_pool = create_command_pool(p_device);
	command_buffer.m_command_buffer = create_command_buffer(p_device, command_buffer.m_command_pool);

	vkFreeCommandBuffers(p_device->m_logical_device, command_buffer.m_command_pool, 1, &command_buffer.m_command_buffer);
	vkDestroyCommandPool(p_device->m_logical_device, command_buffer.m_command_pool, nullptr);
}
