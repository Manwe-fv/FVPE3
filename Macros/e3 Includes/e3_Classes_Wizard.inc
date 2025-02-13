|----------------------------------------------------------------------------|
| Wizard Functions
|----------------------------------------------------------------------------|
SUB check_Harvest
	/if (${autoHarvest} && ${Defined[harvestSpells2D]}) {
    |only harvest if im not in ooc regen
    /if (${Me.CombatState.Equal[COMBAT]}) {
      /declare s int local
      /for s 1 to ${harvestSpells2D.Size[1]}
 /if (${harvestSpells2D.Size[1]}==0) /break
        /call check_Ready "harvestSpells2D" ${s}
        /if (${c_Ready} && ${harvestSpells2D[${s},${iIfs}]}) {
          /if (${check_Mana["harvestSpells2D",${s}]}) {
            /call e3_Cast ${Me.ID} "harvestSpells2D" ${s}
          }
        }
      /if (${ActionTaken}) /break
 /next s
    }
	}
/RETURN
|----------------------------------------------------------------------------|
Sub WIZ_Setup
/if (${Debug}) /echo |- WIZ_Setup ==>
	/call iniToVarV "${Character_Ini},Wizard,Evac Spell" Evac_Spell string outer
	/call iniToVarV "${Character_Ini},Wizard,Auto-Harvest (On/Off)" autoHarvest bool outer
	/call IniToArrayV "${Character_Ini},Wizard,Harvest#" harvestSpells
  /if (${Defined[harvestSpells]}) /call BuildSpellArray "harvestSpells"	"harvestSpells2D"

/if (${Debug}) /echo <== WIZ_Setup -|
/return
|----------------------------------------------------------------------------|
SUB WIZ_MacroSettings
/if (${Debug}) /echo |- WIZ_MacroSettings ==>
	/call WriteToIni "${advSettings_Ini},WIZ Functions,WIZ Function#1" "check_Burns" 1
	/call WriteToIni "${advSettings_Ini},WIZ Functions,WIZ Function#2" "check_Buffs" 1
	/call WriteToIni "${advSettings_Ini},WIZ Functions,WIZ Function#3" "check_AE" 1
	/call WriteToIni "${advSettings_Ini},WIZ Functions,WIZ Function#4" "check_Nukes" 1
	/call WriteToIni "${advSettings_Ini},WIZ Functions,WIZ Function#5" "check_Harvest" 1
	/call WriteToIni "${advSettings_Ini},WIZ Functions,WIZ Function#6" "check_Food" 1
/if (${Debug}) /echo <== WIZ_MacroSettings -|
/RETURN
|----------------------------------------------------------------------------|
SUB WIZ_CharacterSettings
/if (${Debug}) /echo |- WIZ_CharacterSettings ==>
	/call WriteToIni "${Character_Ini},Wizard,Evac Spell"
	/call WriteToIni "${Character_Ini},Wizard,Auto-Harvest (On/Off)" Off
	/call WriteToIni "${Character_Ini},Wizard,Harvest"
	/call WriteToIni "${Character_Ini},Wizard,;---------------------------------"
|Create Manastone Settings
	/call WriteToIni "${Character_Ini},Manastone,Manastone Enabled (On/Off)" Off
	/call WriteToIni "${Character_Ini},Manastone,Min HP" 75
	/call WriteToIni "${Character_Ini},Manastone,Out of Combat MinMana" 85
	/call WriteToIni "${Character_Ini},Manastone,Use in Combat (On/Off)" Off
	/call WriteToIni "${Character_Ini},Manastone,In Combat MinMana" 40
	/call WriteToIni "${Character_Ini},Manastone,DelayBetweenLoops (in milliseconds)" 0
	/call WriteToIni "${Character_Ini},Manastone,;---------------------------------"

/if (${Debug}) /echo <== WIZ_CharacterSettings -|
/RETURN
|----------------------------------------------------------------------------|
Sub WIZ_Aliases
/return
|----------------------------------------------------------------------------|
Sub WIZ_Background_Events
	/if (${Me.Pet.ID} && ${Me.Pet.Race.Name.NotEqual[Gnomework]} && ${MacroQuest.Server.Equal[PEQTGC]}) /pet get lost
/return
