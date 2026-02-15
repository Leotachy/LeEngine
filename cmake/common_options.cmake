

function (LeEngine_target_settings target foldername)
    
	LeEngine_target_settings_toolchain(${target})

    set_property(TARGET ${target} PROPERTY FOLDER ${foldername})

endfunction()
