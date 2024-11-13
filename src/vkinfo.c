/* File: src/vkinfo.c
 * Part of vkinfo <github.com/nesanter/vkinfo>
 *
 * Copyright (C) 2024 Noah Santer <n.ed.santer@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static const char * device_type_string(VkPhysicalDeviceType type)
{
    const char * strings[] = {
        [VK_PHYSICAL_DEVICE_TYPE_OTHER] = "other",
        [VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU] = "integrated GPU",
        [VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU] = "discrete GPU",
        [VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU] = "virtual GPU",
        [VK_PHYSICAL_DEVICE_TYPE_CPU] = "CPU"
    };

    if (type < 0 || type > sizeof(strings) / sizeof(*strings)) {
        return "unknown";
    }
    return strings[type];
}

    /*
    GLFWwindow * window;
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    renderer.window = glfwCreateWindow(800, 600, "vkinfo", NULL, NULL);

    if (!renderer.window) {
        fprintf(stderr, "(ERROR) glfwCreateWindow() failed\n");
        renderer_terminate();
        return RENDERER_INIT_ERROR;
    }
    */
/* initialize the vulkan instance and extensions */
static int vkinfo()
{
    glfwInit();

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "vkinfo",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0), /* TODO */
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };

    /* extensions required by GLFW */
    uint32_t n_glfw_extensions = 0;
    const char ** glfw_extensions =
        glfwGetRequiredInstanceExtensions(&n_glfw_extensions);

    printf("INSTANCE EXTENSIONS REQUIRED BY GLFW\n");
    if (n_glfw_extensions == 0) {
        printf("<none>\n");
    }
    for (size_t i = 0; i < n_glfw_extensions; i++) {
        printf("%s\n", glfw_extensions[i]);
    }
    printf("\n");

    uint32_t n_available_extensions;
    vkEnumerateInstanceExtensionProperties(
            NULL, &n_available_extensions, NULL);
    VkExtensionProperties * available_extensions = malloc(
            sizeof(*available_extensions) * n_available_extensions);
    vkEnumerateInstanceExtensionProperties(
            NULL, &n_available_extensions, available_extensions);

    printf("INSTANCE EXTENSIONS AVAILABLE\n");
    if (n_available_extensions == 0) {
        printf("<none>\n");
    }
    for (size_t i = 0; i < n_available_extensions; i++) {
        printf(
                "%s (spec version %u)\n",
                available_extensions[i].extensionName,
                available_extensions[i].specVersion
            );
    }
    printf("\n");

    free(available_extensions);

    uint32_t n_available_layers;
    vkEnumerateInstanceLayerProperties(&n_available_layers, NULL);
    VkLayerProperties * available_layers = malloc(
        sizeof(*available_layers) * n_available_layers);
    vkEnumerateInstanceLayerProperties(&n_available_layers, available_layers);

    printf("LAYERS AVAILABLE\n");
    if (n_available_layers == 0) {
        printf("<none>\n");
    }
    for (size_t i = 0; i < n_available_layers; i++) {
        printf(
                "%s (spec version %u; version %u; %s)\n",
                available_layers[i].layerName,
                available_layers[i].specVersion,
                available_layers[i].implementationVersion,
                available_layers[i].description
            );
    }
    printf("\n");

    free(available_layers);

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledExtensionCount = n_glfw_extensions,
        .ppEnabledExtensionNames = glfw_extensions
    };

    VkInstance instance;
    VkResult result = vkCreateInstance(&create_info, NULL, &instance);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "ERROR vkCreateInstance() failed\n");
        glfwTerminate();
        return 1;
    }

    uint32_t n_devices;
    vkEnumeratePhysicalDevices(instance, &n_devices, NULL);
    VkPhysicalDevice * devices = malloc(sizeof(*devices) * n_devices);
    vkEnumeratePhysicalDevices(instance, &n_devices, devices);

    if (n_devices == 0) {
        printf("<no physical devices>\n");
    }
    for (size_t i = 0; i < n_devices; i++) {
        VkPhysicalDevice device = devices[i];
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(device, &device_properties);

        printf("DEVICE %s\n", device_properties.deviceName);
        printf("\n");
        printf(
                "  Version: %u (api); %u (driver)\n",
                device_properties.apiVersion,
                device_properties.driverVersion
            );
        printf("  ID: %u (vendor); %u (device)\n",
                device_properties.vendorID,
                device_properties.deviceID
            );
        printf("  Type: %u (%s)\n",
                device_properties.deviceType,
                device_type_string(device_properties.deviceType)
            );
        printf("\n");

        uint32_t n_device_extensions;
        vkEnumerateDeviceExtensionProperties(
                device, NULL, &n_device_extensions, NULL);
        VkExtensionProperties * device_extensions =
            malloc(sizeof(*device_extensions) * n_device_extensions);
        vkEnumerateDeviceExtensionProperties(
                device, NULL, &n_device_extensions, device_extensions);


        printf("  Extensions:\n");
        if (n_device_extensions == 0) {
            printf("  <none>\n");
        }
        for (size_t j = 0; j < n_device_extensions; j++ ) {
            printf(
                    "  %s (spec version %u)\n",
                    device_extensions[j].extensionName,
                    device_extensions[j].specVersion
                );
        }
        printf("\n");

        free(device_extensions);
    }

    free(devices);
    vkDestroyInstance(instance, NULL);
    glfwTerminate();
    return 0;
}

int main(int argc, char ** argv)
{
    (void)argc;
    (void)argv;
    return vkinfo();
}
