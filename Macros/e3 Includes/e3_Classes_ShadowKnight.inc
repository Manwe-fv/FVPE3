|----------------------------------------------------------------------------|
| Shadowknight Functions
|----------------------------------------------------------------------------|
Sub SHD_Setup
/if (${Debug}) /echo |- SHD_Setup ==>
	/call IniToArrayV "${Character_Ini},${Me.Class},LifeTap#" lifeTaps
	/if (${lifeTaps.Size}) /call BuildSpellArray "lifeTaps"	"lifeTaps2D"
/if (${Debug}) /echo <== SHD_Setup -|
/return
|----------------------------------------------------------------------------|
SUB SHD_MacroSettings
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#1" "check_Tanking" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#2" "check_Burns" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#3" "check_lifeTap" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#4" "check_Debuffs" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#5" "check_DoTs" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#6" "check_Nukes" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#7" "check_Buffs" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#8" "check_Pets" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#9" "check_Food" 1
/return
|----------------------------------------------------------------------------|
SUB SHD_CharacterSettings
/if (${Debug}) /echo |- SHD_CharacterSettings ==>
	/call WriteToIni "${Character_Ini},${Me.Class},LifeTap"
 	/call WriteToIni "${Character_Ini},${Me.Class},;---------------------------------"
/if (${Debug}) /echo <== SHD_CharacterSettings -|
/RETURN
|----------------------------------------------------------------------------|
Sub SHD_Aliases
/if (${Debug}) /echo |- SHD_Aliases ==>
/if (${Debug}) /echo <== SHD_Aliases -|
/return
|----------------------------------------------------------------------------|
Sub SHD_Background_Events
/return