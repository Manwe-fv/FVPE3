|-----------------------
|- EVENT_build_HealChain
|---------------------------
|- Creates healChain related variables, and automatically adds characters set to Join Heal Chains.
|---------------------------
#EVENT build_HealChain "#1# tells the raid#*#Build heal chain'"
#EVENT build_HealChain "#1# tell your raid#*#Build heal chain'"
#EVENT build_HealChain "<#1#> Build heal chain"
SUB EVENT_build_HealChain(line, ChatSender)
/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_join_HealChain ==>
	/if (!${Raid.Members}) {
		/if (${ChatSender.Equal[${Me.CleanName}]}) /echo You need to be in a raid to form a heal chain.
	} else {
    /if (!${checkEventArgs[${ChatSender},${line},UZR,""]}) /return
    /varset ChatSender ${c_eventArgChatSender}
    /if (${ChatSender.Equal[${Me.CleanName}]}) {
      | Unload an existing chain
      /if (${Defined[Chain_Moderator]}) /call EVENT_unload_HealChain "PLACEHOLDER" "${Me.CleanName}"
      | Declare chain variables.
      /varset Chain_Moderator ${ChatSender}
      /varset Chain_Count
      /varset Chain_MemberList
      /varset Chain_TimingAdjustment
      /varset Chain_NextHealTimer
      /varset Chain_NextTimingSet FALSE
      /varset Chain_Status BUILDING
      /varset Chain_StartLoc
      /varset Chain_Target
      /varset Chain_LastHealer
      /varset Chain_TankList
      /varset Chain_Timing
    }
    | If I'm set to join heal chains, wait a moment and say "add me"
    /if (${join_HealChain}) {
      /if (${ChatSender.Equal[${Me.CleanName}]}) {
        /call EVENT_add_ChainMember "PLACEHOLDER" "${Me.CleanName}"
      } else {
        /delay 5
        /tell ${ChatSender} Add Me
        /if (!${Me.Gem[Complete Heal]}) {
          /memorize "Complete Heal" ${DefaultGem}
          /delay 2s
        }
      }
    }
  }
/if (${Debug} || ${Debug_HealChain}) /echo <== EVENT_join_HealChain -|
/RETURN

|---------------------------
#EVENT join_HealChain "you told #1#, 'add me'"
SUB EVENT_join_HealChain(line, tellTarget)
/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_join_HealChain ==>
	| Unload an existing chain
	/if (${Defined[Chain_Moderator]}) /call EVENT_unload_HealChain "PLACEHOLDER" "${Me.CleanName}"
	| Declare chain variables.
	/varset Chain_Moderator ${tellTarget}
	/varset Chain_Count
	/varset Chain_MemberList
	/varset Chain_TimingAdjustment
	/varset Chain_NextHealTimer
	/varset Chain_NextTimingSet FALSE
	/varset Chain_Status BUILDING
	/varset Chain_StartLoc
	/varset Chain_Target
	/varset Chain_LastHealer
	/varset Chain_TankList
	/varset Chain_Timing
/if (${Debug} || ${Debug_HealChain}) /echo <== EVENT_join_HealChain -|
/RETURN

|-----------------------
|- EVENT_unload_HealChain
|- Deletes healChain related variables.
|---------------------------
#EVENT unload_HealChain "#1# tells the raid#*#Unload heal chain'"
#EVENT unload_HealChain "#1# tell your raid#*#Unload heal chain'"
#EVENT unload_HealChain "<#1#> Unload heal chain"
#EVENT unload_HealChain "#1# tell #*#, 'remove me'"
SUB EVENT_unload_HealChain(line, ChatSender)
/if (${Debug} || ${Debug_HealChain}) /echo |- unload_HealChain ==>
	| If I'm the ChatSender, or the ChatSender was the chain moderator
	/if (${ChatSender.Equal[${Me.CleanName}]} || ${ChatSender.Equal[${Chain_Moderator}]}) { 
		/if (${line.NotEqual[PLACEHOLDER]}) /echo Unloading heal chain.
		/varset Chain_Moderator
		/varset Chain_Count
		/varset Chain_MemberList
		/varset Chain_TimingAdjustment
		/varset Chain_NextHealTimer
		/varset Chain_NextTimingSet
		/varset Chain_Status
		/varset Chain_StartLoc
		/varset Chain_Target
		/varset Chain_LastHealer
		/varset Chain_TankList
		/varset Chain_Timing
	}
/if (${Debug} || ${Debug_HealChain}) /echo <== unload_HealChain -|
/RETURN

|---------------------------
#EVENT add_ChainMember "#1# tells you, 'Add Me'"
SUB EVENT_add_ChainMember(line, ChatSender)
/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_add_ChainMember ==>
	/if (!${Defined[Chain_Moderator]}) /call healChain_Setup
    /if (!${checkEventArgs[${ChatSender},${line},UZR,""]}) /return
    | If the ChatSender is already part of the HealChain
    /if (${Select[${ChatSender},${Chain_MemberList}]}) {
      /rs [${ChatSender}] is already part of the heal chain.
    } else {
      | Set Chain_MemberList, Chain_Count, and announce new counts to the Chain_Channel.
      /varcalc Chain_Count ${Chain_Count} + 1
      /varset Chain_MemberList ${Chain_MemberList}${ChatSender},
      /rs Added, ${ChatSender} ChainCount: [${Chain_Count}] - MinTiming [${If[${Math.Calc[10/${Chain_Count}]}>${Int[${Math.Calc[10/${Chain_Count}]}]},${Math.Calc[${Int[${Math.Calc[10/${Chain_Count}]}]}+1].Int},${Math.Calc[10/${Chain_Count}].Int}]}s].
      /if (${Chain_Status.Equal[HEALING]}) /rs Set Chain Members to: [${Chain_MemberList.Left[${Math.Calc[${Chain_MemberList.Length} - 1]}]}].
    }
  }
/if (${Debug} || ${Debug_HealChain}) /echo <== EVENT_add_ChainMember -|
/RETURN

|---------------------------
#EVENT remove_ChainMember "#1# tells you, 'Remove me'"
SUB EVENT_remove_ChainMember(line, ChatSender)
/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_remove_ChainMember ==>
	/if (${Defined[Chain_Moderator]} && ${Chain_Moderator.Equal[${Me.CleanName}]}) {
			| If the ChatSender is not part of the HealChain
			/if (!${Select[${ChatSender},${Chain_MemberList}]}) {
				/rs [${ChatSender}] you're not part of the heal chain.
			} else {
				| Set Chain_MemberList, Chain_Count, and announce new counts to the Chain_Channel.
				/declare i int local 1
				/declare new_Count int local
				/declare new_MemberList string local

				:rebuild_MemberList
				/if (${Chain_MemberList.Arg[${i},,].Length}) {
					/if (${Chain_MemberList.Arg[${i},,].NotEqual[${ChatSender}]}) {
						/varset new_MemberList ${new_MemberList}${Chain_MemberList.Arg[${i},,]},
						/varcalc new_Count ${new_Count} + 1
					}
					/varcalc i ${i} + 1
					/goto :rebuild_MemberList
				}
				/varset Chain_MemberList ${new_MemberList}
				/varset Chain_Count ${new_Count}
				/rs Removed, ${ChatSender} ChainCount: [${Chain_Count}] - MinTiming [${If[${Math.Calc[10/${Chain_Count}]}>${Int[${Math.Calc[10/${Chain_Count}]}]},${Math.Calc[${Int[${Math.Calc[10/${Chain_Count}]}]}+1].Int},${Math.Calc[10/${Chain_Count}].Int}]}s].
				/if (${Chain_Timing} && ${Chain_Timing} < ${If[${Math.Calc[10/${Chain_Count}]}>${Int[${Math.Calc[10/${Chain_Count}]}]},${Math.Calc[${Int[${Math.Calc[10/${Chain_Count}]}]}+1].Int},${Math.Calc[10/${Chain_Count}].Int}]}) {
					/rs The heal chain cannot maintain a [${Chain_Timing}s] heal chain with only [${Chain_Count}] members.  Setting chain timing to the mimimum timing of [${If[${Math.Calc[10/${Chain_Count}]}>${Int[${Math.Calc[10/${Chain_Count}]}]},${Math.Calc[${Int[${Math.Calc[10/${Chain_Count}]}]}+1].Int},${Math.Calc[10/${Chain_Count}].Int}]}s].
					/varset Chain_Timing ${If[${Math.Calc[10/${Chain_Count}]}>${Int[${Math.Calc[10/${Chain_Count}]}]},${Math.Calc[${Int[${Math.Calc[10/${Chain_Count}]}]}+1].Int},${Math.Calc[10/${Chain_Count}].Int}]}
				}
				/if (${Chain_Status.Equal[HEALING]}) /rs Set Chain Members to: [${Chain_MemberList.Left[${Math.Calc[${Chain_MemberList.Length} - 1]}]}].
			}
	}
/if (${Debug} || ${Debug_HealChain}) /echo <== EVENT_remove_ChainMember -|
/RETURN


#EVENT set_TankList "#1# tell your raid#*#Set Tank #2#'"
#EVENT set_TankList "#1# tell your raid#*#Set Tanks #2#'"
SUB EVENT_set_TankList(line, ChatSender, tankList_Setting)
/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_set_TankList ==>
	/if (${Chain_Moderator.Equal[${Me.CleanName}]}) {
		/declare i int local 1
		:check_TankList
		/if (${tankList_Setting.Arg[${i},,].Length}) {
			/if (${SpawnCount[pc ${tankList_Setting.Arg[${i},,]}]}) {
				/if (${Select[${tankList_Setting.Arg[${i},,]},${Chain_TankList}]}) {
					/rs [${tankList_Setting.Arg[${i},,]}] is already on the tank list.
				} else {
					/varset Chain_TankList ${Chain_TankList}${Spawn[pc ${tankList_Setting.Arg[${i},,]}].CleanName},
				}
			} else {
				/rs #*# ERROR #*# Could not find pc [${tankList_Setting.Arg[${i},,]}] in ${Zone}.
			}
			/varcalc i ${i} + 1
			/goto :check_TankList
		}
		/if (${Bool[${Chain_TankList}]}) /rs Setting Tank list to: [${Chain_TankList.Left[${Math.Calc[${Chain_TankList.Length} - 1]}]}].
	}
/if (${Debug} || ${Debug_HealChain}) /echo <== EVENT_set_TankList -|
/RETURN


#EVENT lock_TankList "#1# tells the raid#*#Setting Tank list to: [#2#].'"
#EVENT lock_TankList "#1# tell your raid#*#Setting Tank list to: [#2#].'"
SUB EVENT_lock_TankList(line, ChatSender, tankList_Setting)
/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_lock_TankList ==>
	/if (${ChatSender.Equal[${Chain_Moderator}]}) {
		/varset Chain_TankList ${tankList_Setting}
		/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_lock_TankList -| Setting Chain_TankList to [${Chain_TankList}].
	}
/if (${Debug} || ${Debug_HealChain}) /echo <== EVENT_lock_TankList -|
/RETURN


#EVENT set_chainTiming "#1# tell your raid#*#set Timing #2#'"
SUB EVENT_set_chainTiming(line, ChatSender, chainTimer_Setting)
/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_set_chainTiming ==>
	/varset chainTimer_Setting ${Int[${chainTimer_Setting}]}
	/if (${Chain_Moderator.Equal[${Me.CleanName}]}) {
		/if (${chainTimer_Setting} >= ${If[${Math.Calc[10/${Chain_Count}]}>${Int[${Math.Calc[10/${Chain_Count}]}]},${Math.Calc[${Int[${Math.Calc[10/${Chain_Count}]}]}+1].Int},${Math.Calc[10/${Chain_Count}].Int}]}) {
			/rs Setting Chain Timing to: [${chainTimer_Setting}s].
		} else {
			/rs Your Chain Timing cannot be lower than [${If[${Math.Calc[10/${Chain_Count}]}>${Int[${Math.Calc[10/${Chain_Count}]}]},${Math.Calc[${Int[${Math.Calc[10/${Chain_Count}]}]}+1].Int},${Math.Calc[10/${Chain_Count}].Int}]}s].
		}
	}
/if (${Debug} || ${Debug_HealChain}) /echo <== EVENT_set_chainTiming -|
/RETURN


#EVENT lock_chainTiming "#1# tells the raid#*#Setting Chain Timing to: [#2#].'"
#EVENT lock_chainTiming "#1# tell your raid#*#Setting Chain Timing to: [#2#].'"
SUB EVENT_lock_chainTiming(line, ChatSender, chainTimer_Setting)
/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_lock_chainTiming ==>
	/if (${ChatSender.Equal[${Chain_Moderator}]}) {
		/varset Chain_Timing ${chainTimer_Setting}
		/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_lock_chainTiming -| Setting Chain_Timing to [${Chain_Timing}].
	}
/if (${Debug} || ${Debug_HealChain}) /echo <== EVENT_lock_chainTiming -|
/RETURN


#EVENT lock_MemberList "#1# tells the raid#*#Set Chain Members to: [#2#].'"
#EVENT lock_MemberList "#1# tell your raid#*#Set Chain Members to: [#2#].'"
SUB EVENT_lock_MemberList(line, ChatSender, memberList)
/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_lock_MemberList ==>
	/if (${ChatSender.Equal[${Chain_Moderator}]}) {
		/declare i int local 1
		/varset Chain_MemberList ${memberList}
		:nextHealer
		/if (${Chain_MemberList.Arg[${i},,].Length}) {
			/varcalc i ${i}+1
			/goto :nextHealer
		}
		/varset Chain_Count ${Math.Calc[${i}-1]}
		/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_lock_MemberList -| Chain_MemberList= ${Chain_MemberList}
	}
/if (${Debug} || ${Debug_HealChain}) /echo <== EVENT_lock_MemberList -|
/RETURN


#EVENT start_HealChain "#1# tell your raid#*#Start Chain'"
#EVENT start_HealChain "#1# tell your raid#*#Start Heal Chain'"
SUB EVENT_start_HealChain(line, ChatSender)
/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_start_HealChain ==>
	/if (${Chain_Moderator.Equal[${Me.CleanName}]}) {
		| Check for Healers	
		/if (!${Chain_MemberList.Length}) {	
			/rs #*# ERROR #*# You don't have any healers for your heal chain!
		} else {
			| Check for a tank list
			/if (!${Chain_TankList.Length}) {
				/if (${Chain_Moderator.Equal[${Me.CleanName}]}) /rs #*# ERROR #*# You have not specified any tanks to heal!
			} else {
				/rs Set Chain Members to: [${Chain_MemberList.Left[${Math.Calc[${Chain_MemberList.Length} - 1]}]}].
				| Check chain timing
				/if (!${Chain_Timing}) {
					/if (${Chain_Moderator.Equal[${Me.CleanName}]}) /rs You have not specified a chain timing!  Defaulting to [${If[${Math.Calc[10/${Chain_Count}]}>${Int[${Math.Calc[10/${Chain_Count}]}]},${Math.Calc[${Int[${Math.Calc[10/${Chain_Count}]}]}+1].Int},${Math.Calc[10/${Chain_Count}].Int}]}s]
					/varset Chain_Timing ${If[${Math.Calc[10/${Chain_Count}]}>${Int[${Math.Calc[10/${Chain_Count}]}]},${Math.Calc[${Int[${Math.Calc[10/${Chain_Count}]}]}+1].Int},${Math.Calc[10/${Chain_Count}].Int}]}
				}
				/rs Starting a [${Chain_Timing}s] Heal Chain on [${Chain_TankList}]...
			}
		}
	}
/if (${Debug} || ${Debug_HealChain}) /echo <== EVENT_start_HealChain -|
/RETURN


#EVENT engage_ChainHeal "#1# tells the raid#*#Starting a [#2#] Heal Chain on [#3#]...'"
#EVENT engage_ChainHeal "#1# tell your raid#*#Starting a [#2#] Heal Chain on [#3#]...'"
SUB EVENT_engage_ChainHeal(ling, ChatSender, chainTiming_Setting, chainTank_Setting)
/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_engage_ChainHeal ==>
	/if (${ChatSender.Equal[${Chain_Moderator}]}) {
		/varset Chain_Status HEALING
		/if (${Select[${Me.CleanName},${Chain_MemberList}]}) {
			/varset Chain_Timing ${chainTiming_Setting}
			/varset Chain_TankList ${chainTank_Setting}
			/varset Chain_StartLoc ${Me.Loc.Replace[ ,]}
			/varset Chain_LastHealer ${Chain_MemberList.Arg[${Chain_Count},,]}
		}
	}
/if (${Debug} || ${Debug_HealChain}) /echo <== EVENT_engage_ChainHeal -|
/RETURN


#EVENT end_ChainHeal "#1# tells the raid#*#End chain#*#"
#EVENT end_ChainHeal "#1# tell your raid#*#End chain#*#"
#EVENT end_ChainHeal "#1# tells the raid#*#Stop chain#*#"
#EVENT end_ChainHeal "#1# tell your raid#*#Stop chain#*#"
SUB EVENT_end_ChainHeal(line, ChatSender)
/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_end_ChainHeal ==>
	/if (${Defined[Chain_Moderator]}) {
		/if (${Chain_Moderator.Equal[${Me.CleanName}]}) /rs Ending the heal chain.
		/varset Chain_Status PAUSE_CHAIN
	}
/if (${Debug} || ${Debug_HealChain}) /echo <== EVENT_end_ChainHeal -|
/RETURN

| There is a place for the chain to break down in this script
SUB check_healChain
|/if (${Debug} || ${Debug_HealChain}) /echo |- check_healChain ==>
	/call chainHeal_Events
	/if (${Chain_Status.Equal[HEALING]}) {
		/varset ActionTaken TRUE
		| Wait until there is less than 1 second on the next heal timer
    |/if (${Debug} || ${Debug_HealChain}) /echo /if (${Chain_NextHealTimer} < 10)
		/if (${Chain_NextHealTimer} < 10) {
			| Check for a valid tank.
			/call set_ChainTarget
			| If no tanks are found.
      /if (${Debug} || ${Debug_HealChain}) /echo ${Macro.Return}
			/if (${Chain_Target.Equal[NOT_FOUND]}) {
				/rs No tanks were found!  Ending heal chain.
				/varset Chain_Status END_CHAIN
			} else {
				| Check for a valid healer.
				/call set_NextHealer
        /if (${Debug} || ${Debug_HealChain}) /echo ${Macro.Return}
				/if (${Macro.Return.Equal[NOT_FOUND]}) {
					/rs No healers were found!  Ending heal chain.
					/varset Chain_Status END_CHAIN
				} else {
					| If I am the next healer, cast the nextCH.
          /if (${Debug} || ${Debug_HealChain}) /echo /if (${Macro.Return.Equal[${Me.CleanName}]}) - ${Macro.Return}
					/if (${Macro.Return.Equal[${Me.CleanName}]}) {
						/call nextCH
					} else {
						| If I am NOT the next healer, wait to set next heal timing.
						/declare TimingSet_Timer timer local 50
						/varset Chain_NextTimingSet FALSE
						| Call chainHeal_Events until next heal timing is set.
						:check_healEvents
						/call chainHeal_Events
						| If the timing did not get set.
						/if (!${Chain_NextTimingSet}) {
							| If I still have time to wait for the event, goto :check_healEvents
							/if (${TimingSet_Timer}) {
								/goto :check_healEvents
							} else {
								/rs Well, shit. The chain seems to have broken down....
								| the next healer has taken too long to respond
								| Restart chain, and call patch heals
								| users can set a Heal Chain Main Heal, and a Heal Chain Patch Heal
								| the healer called by restart will ignore the patch command, but others will use theirs
							}
						}
					}
				}
			}
		}
	}
|/if (${Debug} || ${Debug_HealChain}) /echo <== check_healChain -|
/RETURN


SUB set_ChainTarget
/if (${Debug} || ${Debug_HealChain}) /echo |- set_ChainTarget ==>
	/declare i int local 1
	/varset Chain_Target NOT_FOUND
	:check_Chain_TankList
	/if (${Chain_TankList.Arg[${i},,].Length}) {
		| Check if the tank is in zone.
		/if (${SpawnCount[pc ${Chain_TankList.Arg[${i},,]}]}) {
			| Check tank range from the Chain_StartLoc.
			/if (${Math.Distance[${Spawn[pc ${Chain_TankList.Arg[${i},,]}].Loc.Replace[ ,]}:${Chain_StartLoc}]} < 100) {
				| If the spawn is in zone, and in range, set Chain_Target.
				/varset Chain_Target ${Spawn[pc ${Chain_TankList.Arg[${i},,]}].CleanName}
				/if (${Debug} || ${Debug_HealChain}) /echo |- set_ChainTarget -| Chain_Target= [${Chain_Target}].
			} else {
				/varcalc i ${i}+1
				/goto :check_Chain_TankList
			}
		} else {
			/varcalc i ${i}+1
			/goto :check_Chain_TankList
		}
	}
/if (${Debug} || ${Debug_HealChain}) /echo <== set_ChainTarget -|
/RETURN ${Chain_Target}



SUB set_NextHealer
/if (${Debug} || ${Debug_HealChain}) /echo |- set_Healer ==>
	/declare nextHealer string local NOT_FOUND
	/declare i int local
	| Declare the counting variable, and set it to the position of the last chain member + 1
  /if (${Debug} || ${Debug_HealChain}) /echo ${Select[${Chain_LastHealer},${Chain_MemberList}]} , ${Chain_LastHealer}, ${Chain_MemberList}
	/declare e int local ${Math.Calc[${Select[${Chain_LastHealer},${Chain_MemberList}]} + 1]}
	/for i 1 to ${Chain_Count}
 /if (${Chain_Count}==0) /break
	| If e was set too high, reset to 1
	/if (!${Chain_MemberList.Arg[${e},,].Length}) /varset e 1
	| Check that the next healer is in zone
	/if (${SpawnCount[pc ${Chain_MemberList.Arg[${e},,]}]}) {
		| Check the distance to the next healer
		/if (${Math.Distance[${Spawn[pc ${Chain_Target}].Loc.Replace[ ,]}:${Spawn[pc ${Chain_MemberList.Arg[${e},,]}].Loc.Replace[ ,]}]} < 100) {
			| If the healer is in zone, and in range, set next healer
			/varset nextHealer ${Chain_MemberList.Arg[${e},,]}
		} else {
			/echo ${Spawn[pc ${Chain_MemberList.Arg[${e},,]}].CleanName} is out of range, checking for another healer...
			/next i
		}
	} else {
		/if (${Debug} || ${Debug_HealChain}) /echo [${Chain_MemberList.Arg[${e},,]}] is not in zone!
		/next i
	}
/if (${Debug} || ${Debug_HealChain}) /echo <== set_Healer -|
/RETURN ${nextHealer}

SUB nextCH
/if (${Debug} || ${Debug_HealChain}) /echo |- nextCH ==>
	/declare adjustment_Timer timer local 100
	/varset Chain_NextTimingSet FALSE
	/delay 10 !${Chain_NextHealTimer}
	/rs ${Time.Time24} [${Me.PctMana}m] Casting ${If[${Me.Class.ShortName.Equal[clr]},CH,KR]} on ${Spawn[pc =${Chain_Target}].Name} next heal in [${Chain_Timing}s]
	| /echo /if (${Chain_TimingAdjustment}) /delay ${Chain_TimingAdjustment}
	| /if (${Chain_TimingAdjustment}) /delay ${Chain_TimingAdjustment}
	/casting "${If[${Me.Class.ShortName.Equal[clr]},Complete Heal,Karana's Renewal]}" "-targetid|${Spawn[pc =${Chain_Target}].ID}" -maxtries|3
	| Check for timing adjustment on next cast
	/if (${castReturn.Equal[SUCCESS]} && ${adjustment_Timer} > 0) /varset Chain_TimingAdjustment ${adjustment_Timer}
/if (${Debug} || ${Debug_HealChain}) /echo <== nextCH -|
/RETURN



#EVENT set_LastHealer "#1# tells the raid#*# #2#:#3#:#4# [#*#] Casting #*# next heal in [#*#]'"
#EVENT set_LastHealer "#1# tell your raid#*# #2#:#3#:#4# [#*#] Casting #*# next heal in [#*#]'"
SUB EVENT_set_LastHealer(line, ChatSender, time_Hour, time_Min, time_Sec)
/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_set_LastHealer ==>
  |/echo time_Hour ${time_Hour} time_Min ${time_Min} time_Sec ${time_Sec}
	/if (${Select[${Me.CleanName},${Chain_MemberList}]}) {
		/declare next_Delay int local

		/varcalc next_Delay ${time_Sec} + ${Chain_Timing}
    /if (${Debug} || ${Debug_HealChain}) /echo next_Delay ${next_Delay}, time_Sec ${time_Sec}, Chain_Timing ${Chain_Timing}
		/varset Chain_NextHealTimer ${Math.Calc[${Math.Calc[${next_Delay} - ${Time.Time24.Arg[3,:]}].Int}*10]}
    /if (${Chain_NextHealTimer}>=600) /varcalc Chain_NextHealTimer ${Chain_NextHealTimer}-600
    /if (${Chain_NextHealTimer}>=60) /varcalc Chain_NextHealTimer ${Chain_NextHealTimer}-60

    /if (${Debug} || ${Debug_HealChain}) /echo |- Chain_NextHealTimer -| set to ${Chain_NextHealTimer}: ${next_Delay} - ${Time.Time24.Arg[3,:]}
    /if (${ChatSender.Equal[You]}) {
      /varset Chain_LastHealer ${Me.CleanName}
    } else {
      /varset Chain_LastHealer ${ChatSender}
    }

		/varset Chain_NextTimingSet TRUE
		
		/if (${Debug} || ${Debug_HealChain}) /echo |- EVENT_set_LastHealer -| Chain_Timer= ${Chain_Timing} :: Chain_LastHealer= ${Chain_LastHealer}
	}
/if (${Debug} || ${Debug_HealChain}) /echo <== EVENT_set_LastHealer -|
/RETURN


|--------------------------------------------------------|
|- healChain_Setup										-|
|--------------------------------------------------------|
|- Imports ini settings from Character Ini,[healChain].	-|
|--------------------------------------------------------|
SUB healChain_Setup
/call iniToVarV "${advSettings_Ini},Debug,Debug HealChain (On/Off)" Debug_HealChain bool outer
/if (${Debug} || ${Debug_HealChain}) /echo |- healChain_Setup ==>
	/if (${Ini[${Character_Ini},Heals,Join Heal Chains (On/Off)].Length}) /call iniToVarV "${Character_Ini},Heals,Join Heal Chains (On/Off)" join_HealChain bool outer
  /declare Chain_Moderator string outer
  /declare Chain_Count int outer
  /declare Chain_MemberList string outer
  /declare Chain_TimingAdjustment int outer
  /declare Chain_NextHealTimer timer outer
  /declare Chain_NextTimingSet bool outer
  /declare Chain_Status string outer
  /declare Chain_StartLoc string outer
  /declare Chain_Target string outer
  /declare Chain_LastHealer string outer
  /declare Chain_TankList string outer
  /declare Chain_Timing int outer
/if (${Debug} || ${Debug_HealChain}) /echo <== healChain_Setup -|
/RETURN

SUB chainHeal_Events
    /doevents set_LastHealer
    /doevents set_ChainTiming
    /doevents lock_ChainTiming
    /doevents lock_MemberList
    /doevents engage_ChainHeal
    /doevents end_ChainHeal
/RETURN

Sub healChain_Background_Events
  |/if (${Debug} || ${Debug_HealChain}) /echo |- healChain_Background_Events ==>
    /doevents build_HealChain
    /doevents join_HealChain
    /doevents set_TankList
    /doevents start_HealChain
    /doevents unload_HealChain
    /doevents end_ChainHeal
    /doevents add_ChainMember
    /doevents remove_ChainMember
  |/if (${Debug} || ${Debug_HealChain}) /echo <== healChain_Background_Events -|
/return

SUB healChain_MacroSettings
	/call WriteToIni "${advSettings_Ini},Debug,Debug HealChain (On/Off)" Off
/RETURN

SUB healChain_CharacterSettings
	/if (${Me.Class.ShortName.Equal[Clr]} || ${Me.Class.ShortName.Equal[Dru]}) /call WriteToIni "${Character_Ini},Heals,Join Heal Chains (On/Off)" Off
/RETURN

Sub healChain_Aliases
/if (${Debug}) /echo |- healChain_Aliases ==>
	/squelch /alias /BuildChain /bc Build Heal Chain
	/noparse /squelch /alias /TankList /rs Set Tank List
	/noparse /squelch /alias /ChainTiming /rs Set Chain Timing
/if (${Debug}) /echo <== healChain_Aliases -|
/return