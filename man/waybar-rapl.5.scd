waybar-rapl(5)

# NAME

waybar - rapl module

# DESCRIPTION

The *rapl* module displays the current processor package power consumption using RAPL (Running Average Power Limit).

# CONFIGURATION

*interval*: ++
	typeof: integer or float ++
	default: 10 ++
	The interval in which the information gets polled. ++
	Minimum value is 0.001 (1ms). Values smaller than 1ms will be set to 1ms.

*format*: ++
	typeof: string  ++
	default: {power}W ++
	The format, how information should be displayed. On {} data gets inserted.

*max-length*: ++
	typeof: integer ++
	The maximum length in character the module should display.

*min-length*: ++
	typeof: integer ++
	The minimum length in characters the module should accept.

*align*: ++
	typeof: float ++
	The alignment of the label within the module, where 0 is left-aligned and 1 is right-aligned. If the module is rotated, it will follow the flow of the text.

*justify*: ++
	typeof: string ++
	The alignment of the text within the module's label, allowing options 'left', 'right', or 'center' to define the positioning.

*rotate*: ++
	typeof: integer ++
	Positive value to rotate the text label (in 90 degree increments).

*states*: ++
	typeof: object ++
	A number of power consumption states which get activated on certain power levels. See *waybar-states(5)*.

*on-click*: ++
	typeof: string  ++
	Command to execute when clicked on the module.

*on-click-middle*: ++
	typeof: string ++
	Command to execute when middle-clicked on the module using mousewheel.

*on-click-right*: ++
	typeof: string ++
	Command to execute when you right-click on the module.

*on-update*: ++
	typeof: string ++
	Command to execute when the module is updated.

*on-scroll-up*: ++
	typeof: string ++
	Command to execute when scrolling up on the module.

*on-scroll-down*: ++
	typeof: string ++
	Command to execute when scrolling down on the module.

*smooth-scrolling-threshold*: ++
	typeof: double ++
	Threshold to be used when scrolling.

*sysfs-dir*: ++
	typeof: string ++
	default: "/sys/class/powercap/intel-rapl" ++
	The path to the RAPL sysfs directory. This allows you to specify a custom location for the RAPL sysfs files if they are not in the default location.

*tooltip*: ++
	typeof: bool ++
	default: true ++
	Option to disable tooltip on hover.

*expand*: ++
	typeof: bool ++
	default: false ++
	Enables this module to consume all left over space dynamically.

# FORMAT REPLACEMENTS

*{power}*: Current processor package power consumption in watts.

# EXAMPLES

Basic configuration:

```
"rapl": {
	"interval": 2,
	"format": "{power}W ⚡",
	"max-length": 10
}
```

With states:

```
"rapl": {
	"interval": 2,
	"format": "{power}W ⚡",
	"states": {
		"warning": 30,  // 30W
		"critical": 65  // 65W
	}
}
```

With custom sysfs directory:

```
"rapl": {
	"interval": 2,
	"format": "{power}W ⚡",
	"sysfs-dir": "/sys/class/powercap/custom-rapl-location"
}
```

# STYLE

- *#rapl*