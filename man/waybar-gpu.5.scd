waybar-gpu(5)

# NAME

waybar - gpu module

# DESCRIPTION

The *gpu* module displays the GPU usage percentage and VRAM usage.

# CONFIGURATION

Addressed by *gpu*

*format*: ++
	typeof: string ++
	default: *{gpuUsage}%* ++
	The format, how information should be displayed.

*format-alt*: ++
	typeof: string ++
	The format, how information should be displayed when the module is clicked. If not specified, the format will be used.

*format-alt-<state>*: ++
	typeof: string ++
	The format, how information should be displayed when the module is clicked and in a specific state. If not specified, the format-alt will be used.

*format-<state>*: ++
	typeof: string ++
	The format, how information should be displayed if a specific state is reached.
	See *format-icons* for available states.

*gpu-usage-path*: ++
	typeof: string ++
	default: */sys/class/drm/card1/device/gpu_busy_percent* ++
	The path to the file containing GPU usage percentage.

*vram-used-path*: ++
	typeof: string ++
	default: */sys/class/drm/card1/device/mem_info_vram_used* ++
	The path to the file containing VRAM usage in bytes.

*tooltip-format*: ++
	typeof: string ++
	default: *GPU: {gpuUsage}% VRAM: {vramUsed:.2f}GB* ++
	The format, how information should be displayed in the tooltip.

*interval*: ++
	typeof: integer ++
	default: 30 ++
	The interval in seconds for updating the module.

*format-icons*: ++
	typeof: array ++
	Based on the current gpu usage, the corresponding icon gets selected.
	The array is composed of a list of labels, which are state ranges.
	See *icons*.

# FORMAT REPLACEMENTS

*{gpuUsage}*: GPU usage percentage.

*{vramUsed}*: VRAM usage in gigabytes (GB).

*{vramUsedRaw}*: Raw VRAM usage value as read from the file.

*{icon}*: Icon corresponding to the current GPU usage state.

# EXAMPLES

```
"gpu": {
    "format": "{icon} {gpuUsage}%",
    "format-icons": ["▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"],
    "tooltip-format": "GPU: {gpuUsage}% VRAM: {vramUsed:.2f}GB"
}
```

# STYLE

- *#gpu*
- *#gpu-good*
- *#gpu-warning*
- *#gpu-critical*