|----------------------------------------------------------------------------|
| Enchanter Functions
|----------------------------------------------------------------------------|
#event toggleRune "<#1#> AutoRune #2#"
#event toggleRune "#1# tells you, 'AutoRune #2#'"
sub event_toggleRune(line, ChatSender, runeToggle)
  /if (${Me.Class.ShortName.Equal[ENC]}) {
    /if (${Debug} || ${Debug_Ench}) /echo |- toggleRune ==>
    /if (${runeToggle.Equal[on]}) {
      /if (!${Bool[${runeArray2D.Size[1]}]}) /docommand ${ChatToggle} I do not have a rune spell defined
      /varset autoRune TRUE
      /docommand ${ChatToggle} << Auto Rune Enabled >>
    } else {
      /varset autoRune FALSE
      /docommand ${ChatToggle} << Auto Rune Disabled >>
    }
    /if (${Debug}) /echo <== toggleRune -|
  }
/return
|----------------------------------------------------------------------------|
#event add_RuneTarget "<#1#> Rune On #2#"
#event add_RuneTarget "#1# tells you, 'Rune On #2#'"
sub event_add_RuneTarget(line, ChatSender, RuneTarget)
/if (${Debug}) /echo |- event_add_RuneTarget ==>
  /if (${Me.Class.ShortName.Equal[ENC]}) {
    /if (!${SpawnCount[pc =${RuneTarget}]}) {
      /docommand ${ChatToggle} I don't see any PCs matching ${RuneTarget}...
    } else /if (!${Defined[runeTargets]}) {
        /declare runeTargets[1] string outer
        /varset runeTargets[1] ${Spawn[pc =${RuneTarget}].CleanName}
        /docommand ${ChatToggle} Maintaining Rune on ${RuneTarget}
    } else {
        /declare i int local
        /for i 1 to ${runeTargets.Size}
 /if (${runeTargets.Size}==0) /break
          /if (${runeTargets[${i}].Equal[${Spawn[pc =${RuneTarget}].CleanName}]}) {
            /docommand ${ChatToggle} I am already maintaining rune ${RuneTarget}
            /return
          }
        /next i

        /call BuildArray runeTargets ${RuneTarget}
        /docommand ${ChatToggle} Maintaining Rune on ${RuneTarget}
    }
  }
/if (${Debug}) /echo <== add_RuneTarget -|
/return
|----------------------------------------------------------------------------|
#event remove_RuneTarget "<#1#> Rune Off #2#"
#event remove_RuneTarget "#1# tells you, 'Rune off #2#'"
sub event_remove_RuneTarget(line, ChatSender, RuneTarget)
/if (${Debug} || ${Debug_Ench}) /echo |- remove_RuneTarget ==>
  /if (${Me.Class.ShortName.Equal[ENC]}) {
    /if (${Defined[runeTargets]}) {
      /if (${Defined[RuneTarget]}) {
        /declare i int local
        /for i 1 to ${runeTargets.Size}
 /if (${runeTargets.Size}==0) /break
          /if (${runeTargets[${i}].Equal[${Spawn[pc =${RuneTarget}].CleanName}]}) {
            /docommand ${ChatToggle} No longer maintaining Rune on [${runeTargets[${i}]}].
            /call RemoveArrayElement runeTargets "${runeTargets[${i}]}"
          }
        /next i
      } else {
        /docommand ${ChatToggle} Ending runes on all targets
        /deletevar runeTargets
      }
    }
  }
/if (${Debug}) /echo <== remove_RuneTarget -|
/return
|----------------------------------------------------------------------------|
Sub check_rune
/if (${Debug} || ${Debug_Ench}) /echo |- check_rune ==>
/if (${autoRune}) {
  /declare currentBot string local NULL
  /declare currentSpellIndx int local
  /declare t int local
  /declare s int local
  /for t 1 to ${runeTargets.Size}
 /if (${runeTargets.Size}==0) /break
    /for t 1 to ${runeTargets.Size}
 /if (${runeTargets.Size}==0) /break
      /if (${Bool[${NetBots[${runeTargets[${t}]}].InZone}]} && !${Bool[${NetBots[${runeTargets[${t}]}].Buff.Find[${runeArray2D[${s},${iCastID}]} ]}]}) {
        /call check_Ready "runeArray2D" ${s}
        /if (${c_Ready}) {
          /call check_Distance
          /if (${check_Distance[${NetBots[${runeTargets[${t}]}].ID},${runeArray2D[${s},${iMyRange}]}]}) {
            /if (${check_Mana["runeArray2D",${s}]}) {
              /call e3_Cast ${NetBots[${runeTargets[${t}]}].ID} "runeArray2D" ${s}
            }
          }
        }
      }
    /if (${Bool[${currentBot}]}) /break
    /next s
    /if (${Bool[${currentBot}]}) /break
  /next t
}
/if (${Debug}) /echo <== check_rune -|
/return
|----------------------------------------------------------------------------|
#event charmOn "<#1#> Charm on #2#"
#event charmOn "#1# tells you, 'Charm on #2#'"
sub event_charmOn(line, ChatSender, targetID)
  /if (${Me.Class.ShortName.Equal[ENC]} || ${Me.Class.ShortName.Equal[BRD]}) {
    /if (${Debug} || ${Debug_Ench}) /echo |-  charmOn ==>
    |&& (!${Spawn[id ${targetID}].Named} || ${Spawn[id ${targetID}].CleanName.Equal[Coerced Lieutenant]})
      /if ((${Spawn[id ${targetID}].Distance} < 240  && ${SpawnCount[npc id ${targetID}]}>0) || ${targetID}==${Me.Pet.ID}) {
        /varset charmTarget ${targetID}
        /varset doCharm TRUE
        /docommand ${ChatToggle} << Charm enabled ${Spawn[id ${charmTarget}].CleanName} >>
      } else {
        /docommand ${ChatToggle} << ${Spawn[id ${targetID}].CleanName} is not eligible for charm >>
      }
    /if (${Debug}) /echo <== charmOn -|
  }
/return
|----------------------------------------------------------------------------|
#event charmOff "<#1#> Charm Off"
#event charmOff "#1# tells you, 'Charm Off'"
sub event_charmOff(line, ChatSender)
/if (${Debug} || ${Debug_Ench}) /echo |- charmOff ==>
  /if (${Me.Class.ShortName.Equal[ENC]} || ${Me.Class.ShortName.Equal[BRD]}) {
      /if (${doCharm}) {
        /docommand ${ChatToggle} << Charm Disabled ${Spawn[id ${charmTarget}].CleanName} >>
        /varset doCharm FALSE
        /varset charmTarget 0
      }
    /if (${Debug}) /echo <== charmOff -|
  }
/return
|----------------------------------------------------------------------------|
#event toggleMez "<#1#> Mez #2#"
#event toggleMez "#1# tells you, 'Mez #2#'"
sub event_toggleMez(line, ChatSender, mezToggle)
  /if (${Me.Class.ShortName.Equal[ENC]} || ${Me.Class.ShortName.Equal[BRD]}) {
    /if (${Debug} || ${Debug_Ench}) /echo |- remove_RuneTarget ==>
      /if (${mezToggle.Equal[on]}) {
        /varset autoMez TRUE
        /docommand ${ChatToggle} << Mezzing Enabled >>
      } else {
        /varset autoMez FALSE
        /docommand ${ChatToggle} << Mezzing Disabled >>
    }
    /if (${Debug}) /echo <== remove_RuneTarget -|
  }
/return
|----------------------------------------------------------------------------|
Sub check_charm
/if (${Debug} || ${Debug_Ench}) /echo |- check_Charm ==>
/if (${Defined[charmArray2D]}) {
  /declare s int local
  /declare charmDuration int local
  /if (${doCharm} && ${Bool[${charmTarget}]}) {
    /if (${SpawnCount[npc id ${charmTarget}]}==0 && ${charmTarget}!=${Me.Pet.ID}) {
      /docommand ${ChatToggle} Disabling Charm: ${charmTarget} is no longer a valid npc
      /varset doCharm FALSE
      /varset charmTarget 0
      /return
    }
    /if (${Me.Pet.Race.Name.Equal[Invisible Man]}) {
      /pet get lost
      /delay 1s !${Me.Pet.ID}
    }
    /if (!${Bool[${Me.Pet.ID}]}) {
      /bc [+r+] << ${Spawn[id ${charmTarget}].CleanName} is not Charmed! >> [+r+]
      /for s 1 to ${charmArray2D.Size[1]}
 /if (${charmArray2D.Size[1]}==0) /break
      /call check_Ready "charmArray2D" ${s}
      /if (${c_Ready}) {
        /if (${check_Mana["charmArray2D",${s}]}) {
          /if (${check_Distance[${charmTarget},${charmArray2D[${s},${iMyRange}]}]}) {
            /docommand ${ChatToggle} << Charming ${charmTarget} ${Spawn[id ${charmTarget}].CleanName} with ${charmArray2D[${s},${iCastName}]} >>
            /call e3_Cast ${charmTarget} "charmArray2D" "${s}"
            /if (${Bool[${Me.Pet.ID}]}) {
              /bc [+g+] >> Charmed ${Spawn[id ${charmTarget}].CleanName} <<
              /timed 5 /pet back off
              /timed 10 /pet follow
            } else {
              /if (${castReturn.Equal[CAST_IMMUNE]}) {
                /varset doCharm FALSE
                /varset charmTarget 0
                /docommand ${ChatToggle} << ${Spawn[id ${charmTarget}].CleanName} is immune, turning off charm >>
              }
            }
          }
        }
      }
      /if (${Bool[${Me.Pet.ID}]}) /break 
    /next s
    } else {
    |accounce time left on charm
      /for s 1 to ${charmArray2D.Size[1]}
 /if (${charmArray2D.Size[1]}==0) /break
        /if (${Bool[${Me.PetBuff[${charmArray2D[${s},${iSpellName}]}]}]}) {
          /varset charmDuration ${Pet.BuffDuration[${charmArray2D[${s},${iSpellName}]}].TotalSeconds}
          |/echo charmduration ${charmDuration} timer ${charmBuffTimer}
          /if (${charmDuration} > 60) {
            /if (!${Bool[${charmBuffTimer}]}) {
              /bc [+g+] My charm fades in ${charmDuration} sec
              /varset charmBuffTimer 18s
            }
          } else /if (${charmDuration} <= 60 && ${charmDuration} > 9) {
            /if (!${Bool[${charmBuffTimer}]}) {
              /bc [+y+] My charm fades in ${charmDuration} sec
              /varset charmBuffTimer 6s
            }
          } else /if (${charmDuration} <= 9) {
            /if (!${Bool[${charmBuffTimer}]}) {
              /bc [+r+] My charm fades in ${charmDuration} sec
              /varset charmBuffTimer 1s
              }
          }
        }
      /next s
    }
  }
}
/if (${Debug} || ${Debug_Ench}) /echo <== check_Charm -|
/return
|----------------------------------------------------------------------------|
| Mez mobs in ae radius that are not assist target
sub check_mez
/if (${Debug} || ${Debug_Ench}) /echo |- check_Mez ==>
  /if (${autoMez}) {
    /declare i int local
    /declare s int local
    /declare mezTarg int local
    /if (!${SpawnCount[playerstate 4 npc targetable radius ${AE_Radius}]}) /return
    /for i 1 to ${SpawnCount[playerstate 4 npc targetable radius ${AE_Radius}]}
 /if (${SpawnCount[playerstate 4 npc targetable radius ${AE_Radius}]}==0) /break
      /varset mezTarg ${NearestSpawn[${i},playerstate 4 npc targetable radius ${AE_Radius}].ID}
      /call TrueTarget ${mezTarg}
      /delay 1s ${Target.BuffsPopulated}
      /if (${Select[${AssistTarget},${mezTarg}]}==0 && (!${Bool[${Target.Mezzed}]} || ${Target.Mezzed.Duration.TotalSeconds} < 8)) {
        /for s 1 to ${mezArray2D.Size[1]}
 /if (${mezArray2D.Size[1]}==0) /break
          /call check_Ready "mezArray2D" ${s}
          /if (${c_Ready}) {
            /if (${check_Mana["mezArray2D",${s}]}) {
              /if (${check_Distance[${mezTarg},${mezArray2D[${s},${iMyRange}]}]}) {
                /docommand ${ChatToggle} << Mezzing ${mezTarg} with ${mezArray2D[${s},${iCastName}]} >>
                /call e3_Cast ${mezTarg} "mezArray2D" "${s}"
              }
            }
          }
        /if (${ActionTaken}) /break
 /next s
      }
    /if (${ActionTaken}) /break
 /next i
  }
/if (${Debug}) /echo <== check_Mez -|
/return
|----------------------------------------------------------------------------|
Sub ENC_Setup
  /declare Debug_Ench bool outer FALSE
  /declare autoRune bool outer FALSE
  /declare autoMez bool outer FALSE

	/if (${Ini[${Character_Ini},Enchanter,AutoRune].Length}) /call iniToVarV "${Character_Ini},Enchanter,AutoRune (On/Off)" autoRune bool outer
	/if (${Ini[${Character_Ini},Enchanter,AutoMez (On/Off)].Length}) /call iniToVarV "${Character_Ini},Enchanter,AutoMez (On/Off)" autoMez bool outer
	/if (${Ini[${Character_Ini},Enchanter,GatherMana Pct].Length}) /call iniToVarV "${Character_Ini},Enchanter,GatherMana Pct" gatherManaPCT int outer

  /call IniToArrayV "${Character_Ini},Enchanter,RuneTarget#"  runeTargets
  /call IniToArrayV "${Character_Ini},Enchanter,RuneSpell#"   runeArray
  /call IniToArrayV "${Character_Ini},Enchanter,Mez#"         mezArray
  /call IniToArrayV "${Character_Ini},Enchanter,Charm#"       charmArray
  /if (${Bool[${mezArray.Size}]}) {
    /call BuildSpellArray "mezArray"	"mezArray2D"
  } else {
    /if (${Defined[autoMez]}) /varset autoMez FALSE
  }
  /if (${Bool[${runeArray.Size}]}) {
    /call BuildSpellArray "runeArray"	"runeArray2D"
  } else {
    /if (${Defined[autoRune]}) /varset autoRune FALSE
  }
  /declare charmTarget int outer
  /if (${Bool[${charmArray.Size}]}) {
    /call BuildSpellArray "charmArray"	"charmArray2D"
    /declare doCharm bool outer TRUE
    /declare charmBuffTimer timer outer
    |/declare charmTimer timer outer
  } else {
    /declare doCharm bool outer FALSE
  }
/return
|----------------------------------------------------------------------------|
Sub ENC_Background_Events
	/doevents charmTarget
	/doevents toggleMez
	/doevents charmOn
	/doevents charmOff
	/doevents toggleRune
	/doevents add_RuneTarget
	/doevents remove_RuneTarget
	/if (${Assisting} && ${Bool[${gatherManaPCT}]} && ${Me.PctMana} < ${gatherManaPCT} && ${Me.AltAbilityReady[Gather Mana]} && ${Me.CombatState.Equal[COMBAT]}) /call castSimpleSpell "Gather Mana" 0
/return
|----------------------------------------------------------------------------|
SUB ENC_MacroSettings
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#1" "check_Charm" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#2" "check_Mez" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#3" "check_Rune" 1
  /call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#4" "check_Debuffs" 1
  /call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#5" "check_Buffs" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#6" "check_Burns" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#7" "check_DoTs" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#8" "check_AE" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#9" "check_Nukes" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#10" "check_Pets" 1
  /call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#11" "check_Food" 1
/RETURN
|----------------------------------------------------------------------------|
SUB ENC_CharacterSettings
	/call WriteToIni "${Character_Ini},Enchanter,AutoRune (On/Off)" Off
	/call WriteToIni "${Character_Ini},Enchanter,AutoMez (On/Off)" Off
	/call WriteToIni "${Character_Ini},Enchanter,RuneTarget"
	/call WriteToIni "${Character_Ini},Enchanter,RuneSpell"
  /call WriteToIni "${Character_Ini},Enchanter,Mez"
	/call WriteToIni "${Character_Ini},Enchanter,Charm"
  /call WriteToIni "${Character_Ini},Enchanter,GatherMana Pct" 10
  /call WriteToIni "${Character_Ini},Enchanter,;---------------------------------"
|Create Manastone Settings
	/call WriteToIni "${Character_Ini},Manastone,Manastone Enabled (On/Off)" Off
	/call WriteToIni "${Character_Ini},Manastone,Min HP" 75
	/call WriteToIni "${Character_Ini},Manastone,Out of Combat MinMana" 85
	/call WriteToIni "${Character_Ini},Manastone,Use in Combat (On/Off)" Off
	/call WriteToIni "${Character_Ini},Manastone,In Combat MinMana" 40
	/call WriteToIni "${Character_Ini},Manastone,DelayBetweenLoops (in milliseconds)" 0
  /call WriteToIni "${Character_Ini},Manastone,;---------------------------------"
/RETURN
|----------------------------------------------------------------------------|
Sub ENC_Aliases
	/squelch /alias /runeon /bc AutoRune on
	/squelch /alias /runeoff /bc AutoRune off
  /squelch /alias /addrune /bc Rune On
  /squelch /alias /remrune /bc Rune Off
  /noparse /squelch /alias /charmon /bc Charm on ${Target.ID}
  /noparse /squelch /alias /charmoff /bc Charm off
  /squelch /alias /mezon /bc Mez on
  /squelch /alias /mezoff /bc Mez off
/return