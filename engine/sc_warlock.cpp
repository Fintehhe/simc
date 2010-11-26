// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================
#include "simulationcraft.h"

/*
 * To Do:
 * - Check if curse_of_elements is counted for affliction_effects in every possible situation
 *  (optimal_raid=1, or with multiple wl's, dk's/druis), it enhances drain soul damage.
 * - Bane of Agony and haste scaling looks strange (and is crazy as hell, we'll leave it at average base_td for now)
 * - Seed of Corruption with Soulburn: Trigger Corruptions
 * - Execute felguard:felstorm by player, not the pet
 * - Verify if the current incinerate bonus calculation is correct
 * - Investigate pet mp5 - probably needs to scale with owner int
 * - Dismissing a pet drops aura -> demonic_pact. Either don't let it dropt if there is another pet alive, or recheck application when dropping it.
 */

// ==========================================================================
// Warlock
// ==========================================================================


namespace pet_stats {

struct _stat_list_t {
  int id;
  double stats[ BASE_STAT_MAX ];
};
  // Base Stats, same for all pets. Depend on level
  static const _stat_list_t pet_base_stats[]=
  {
          // str, agi,  sta, int, spi,   hp,  mana, mcrit/agi, scrit/int, d/agi, mcrit, scrit, mp5, spi_reg
    { 80, {  314, 226,  328, 150, 209,    0,     0,         0,         0,     0,     0,     0,   0,       0 } },
    { 81, {  345, 297,  333, 151, 212,    0,     0,         0,         0,     0,     0,     0,   0,       0 } },
    { 85, {  453, 883,  353, 159, 225,    0,     0,         0,         0,     0,     0,     0,   0,       0 } },
    { 0, { 0 } }
  };

  static const _stat_list_t imp_base_stats[]=
  {
          // str, agi,  sta, int, spi,     hp,  mana, mcrit/agi, scrit/int, d/agi, mcrit, scrit, mp5, spi_reg
    { 80, {    0,   0,    0,   0,    0,  5026, 17628,         0,         0,     0,     0,     0,   0,       0 } },
    { 85, {    0,   0,    0,   0,    0,  5026, 31607,         0,         0,     0,     0,     0,   0,       0 } },
    { 0, { 0 } }
  };

  static const _stat_list_t felguard_base_stats[]=
  {
          // str, agi,  sta, int, spi,     hp,  mana, mcrit/agi, scrit/int, d/agi, mcrit, scrit, mp5, spi_reg
    { 80, {    0,   0,    0,   0,    0,  5395,  9109,         0,         0,     0,     0,     0,   0,       0 } },
    { 85, {    0,   0,    0,   0,    0,  5395, 19072,         0,         0,     0,     0,     0,   0,       0 } },
    { 0, { 0 } }
  };

  static const _stat_list_t felhunter_base_stats[]=
  {
          // str, agi,  sta, int, spi,     hp,  mana, mcrit/agi, scrit/int, d/agi, mcrit, scrit, mp5, spi_reg
    { 80, {    0,   0,    0,   0,    0,  5395,  9109,         0,         0,     0,     0,     0,   0,       0 } },
    { 85, {    0,   0,    0,   0,    0,  5395, 19072,         0,         0,     0,     0,     0,   0,       0 } },
    { 0, { 0 } }
  };

  static const _stat_list_t succubus_base_stats[]=
  {
          // str, agi,  sta, int, spi,     hp,  mana, mcrit/agi, scrit/int, d/agi, mcrit, scrit, mp5, spi_reg
    { 80, {    0,   0,    0,   0,    0,  4530,  9109,         0,         0,     0,     0,     0,   0,       0 } },
    { 85, {    0,   0,    0,   0,    0,  5640, 19072,         0,         0,     0,     0,     0,   0,       0 } },
    { 0, { 0 } }
  };

  static const _stat_list_t infernal_base_stats[]=
  {
          // str, agi,  sta, int, spi,     hp,  mana, mcrit/agi, scrit/int, d/agi, mcrit, scrit, mp5, spi_reg
    { 80, {    0,   0,    0,   0,    0,     0,     0,         0,         0,     0,     0,     0,   0,       0 } },
    { 85, {    0,   0,    0,   0,    0,     0,     0,         0,         0,     0,     0,     0,   0,       0 } },
    { 0, { 0 } }
  };

  static const _stat_list_t doomguard_base_stats[]=
  {
          // str, agi,  sta, int, spi,     hp,  mana, mcrit/agi, scrit/int, d/agi, mcrit, scrit, mp5, spi_reg
    { 80, {    0,   0,    0,   0,    0,     0,     0,         0,         0,     0,     0,     0,   0,       0 } },
    { 85, {    0,   0,    0,   0,    0,     0,     0,         0,         0,     0,     0,     0,   0,       0 } },
    { 0, { 0 } }
  };

  static const _stat_list_t ebon_imp_base_stats[]=
  {
          // str, agi,  sta, int, spi,     hp,  mana, mcrit/agi, scrit/int, d/agi, mcrit, scrit, mp5, spi_reg
    { 80, {    0,   0,    0,   0,    0,     0,     0,         0,         0,     0,     0,     0,   0,       0 } },
    { 85, {    0,   0,    0,   0,    0,     0,     0,         0,         0,     0,     0,     0,   0,       0 } },
    { 0, { 0 } }
  };

  static const _stat_list_t voidwalker_base_stats[]=
  {
          // str, agi,  sta, int, spi,     hp,  mana, mcrit/agi, scrit/int, d/agi, mcrit, scrit, mp5, spi_reg
    { 80, {    0,   0,    0,   0,    0,  6131,  9109,         0,         0,     0,     0,     0,   0,       0 } },
    { 85, {    0,   0,    0,   0,    0,  6131, 19072,         0,         0,     0,     0,     0,   0,       0 } },
    { 0, { 0 } }
  };

  struct _weapon_list_t
  {
    int id;
    double min_dmg, max_dmg;
    double swing_time;
  };

  static const _weapon_list_t imp_weapon[]=
  {
    { 81, 116.7, 176.7, 2.0 },
    { 0, 0, 0, 0 }
  };
  static const _weapon_list_t felguard_weapon[]=
  {
    { 80, 824.6, 824.6, 2.0 },
    { 81, 848.7, 848.7, 2.0 },
    { 85, 926.3, 926.3, 2.0 },
    { 0, 0, 0, 0 }
  };
  static const _weapon_list_t felhunter_weapon[]=
  {
    { 80, 824.6, 824.6, 2.0 },
    { 81, 848.7, 848.7, 2.0 },
    { 85, 926.3, 926.3, 2.0 },
    { 81, 678.4, 1010.4, 2.0 },
    { 0, 0, 0, 0 }
  };
  static const _weapon_list_t succubus_weapon[]=
  {
	  { 80, 824.6, 824.6, 2.0 },
    { 81, 848.7, 848.7, 2.0 },
    { 85, 926.3, 926.3, 2.0 },
    { 0, 0, 0, 0 }
  };
  static const _weapon_list_t infernal_weapon[]=
  { 
    { 80, 924.0, 924.0, 2.0 }, //Rough numbers
    { 85, 1072.0, 1072.0, 2.0 }, //Rough numbers
    { 0, 0, 0, 0 }
  };
  static const _weapon_list_t doomguard_weapon[]=
  {
    { 0, 0, 0, 0 }
  };
  static const _weapon_list_t ebon_imp_weapon[]=
  {
    { 80, 956.0, 956.0, 2.0 }, //Rough numbers
    { 85, 1110.0, 1110.0, 2.0 }, //Rough numbers
    { 0, 0, 0, 0 }
  };
  static const _weapon_list_t voidwalker_weapon[]=
  {
    { 80, 824.6, 824.6, 2.0 },
    { 81, 848.7, 848.7, 2.0 },
    { 85, 926.3, 926.3, 2.0 },
    { 0, 0, 0, 0 }
  };
}



struct warlock_pet_t;
struct warlock_main_pet_t;
struct warlock_guardian_pet_t;

struct warlock_t : public player_t
{
  // Active Pet
  warlock_main_pet_t* active_pet;

  // Dots
  dot_t*  dots_corruption;
  dot_t*  dots_unstable_affliction;
  dot_t*  dots_bane_of_agony;
  dot_t*  dots_bane_of_doom;
  dot_t*  dots_immolate;
  dot_t*  dots_drain_life;
  dot_t*  dots_drain_soul;
  dot_t*  dots_shadowflame;
  dot_t*  dots_curse_of_elements;
  dot_t*  dots_burning_embers;

  // Buffs
  buff_t* buffs_backdraft;
  buff_t* buffs_decimation;
  buff_t* buffs_demonic_empowerment;
  buff_t* buffs_empowered_imp;
  buff_t* buffs_eradication;
  buff_t* buffs_fel_armor;
  buff_t* buffs_haunted;
  buff_t* buffs_shadow_embrace;
  buff_t* buffs_metamorphosis;
  buff_t* buffs_molten_core;
  buff_t* buffs_shadow_trance;
  buff_t* buffs_hand_of_guldan;
  buff_t* buffs_improved_soul_fire;
  buff_t* buffs_dark_intent_feedback;
  buff_t* buffs_soulburn;
  buff_t* buffs_demon_soul_imp;
  buff_t* buffs_demon_soul_felguard;
  buff_t* buffs_demon_soul_felhunter;
  buff_t* buffs_demon_soul_succubus;
  buff_t* buffs_demon_soul_voidwalker;
  buff_t* buffs_bane_of_havoc;
  buff_t* buffs_searing_pain_soulburn;
  buff_t* buffs_tier10_4pc_caster;
  buff_t* buffs_tier11_4pc_caster;

  // Cooldowns
  cooldown_t* cooldowns_metamorphosis;
  cooldown_t* cooldowns_infernal;
  cooldown_t* cooldowns_doomguard;

  // Talents

  // Affliction
  passive_spell_t* talent_unstable_affliction;
  talent_t* talent_doom_and_gloom; // done
  talent_t* talent_improved_life_tap; // done
  talent_t* talent_improved_corruption; // done
  talent_t* talent_jinx;
  talent_t* talent_soul_siphon; // done
  talent_t* talent_siphon_life; // done
  talent_t* talent_eradication; // done
  talent_t* talent_soul_swap;
  talent_t* talent_shadow_embrace; // done
  talent_t* talent_deaths_embrace; // done
  talent_t* talent_nightfall; // done
  talent_t* talent_soulburn_seed_of_corruption;
  talent_t* talent_everlasting_affliction; // done
  talent_t* talent_pandemic; // done
  talent_t* talent_haunt; // done

  // Demonology
  passive_spell_t* talent_summon_felguard;
  talent_t* talent_demonic_embrace; // done
  talent_t* talent_dark_arts; // done
  talent_t* talent_mana_feed; // done
  talent_t* talent_demonic_aegis;
  talent_t* talent_master_summoner;
  talent_t* talent_impending_doom;
  talent_t* talent_demonic_empowerment;
  talent_t* talent_molten_core; // done
  talent_t* talent_hand_of_guldan;
  talent_t* talent_aura_of_foreboding;
  talent_t* talent_ancient_grimoire;
  talent_t* talent_inferno;
  talent_t* talent_decimation; // done
  talent_t* talent_cremation;
  talent_t* talent_demonic_pact;
  talent_t* talent_metamorphosis; // done

  // Destruction
  passive_spell_t* talent_conflagrate;
  talent_t* talent_bane; // done
  talent_t* talent_shadow_and_flame; // done
  talent_t* talent_improved_immolate;
  talent_t* talent_improved_soul_fire;
  talent_t* talent_emberstorm; // done
  talent_t* talent_improved_searing_pain;
  talent_t* talent_backdraft; // done
  talent_t* talent_shadowburn; // done
  talent_t* talent_burning_embers;
  talent_t* talent_soul_leech; // done
  talent_t* talent_fire_and_brimstone; // done
  talent_t* talent_shadowfury; // done
  talent_t* talent_empowered_imp; // done
  talent_t* talent_bane_of_havoc;
  talent_t* talent_chaos_bolt; // done

  struct passive_spells_t
  {
    passive_spell_t* shadow_mastery;
    passive_spell_t* demonic_knowledge;
    passive_spell_t* cataclysm;
    passive_spell_t* doom_and_gloom;
    passive_spell_t* pandemic;
    passive_spell_t* nethermancy;

  };
  passive_spells_t passive_spells;

  struct mastery_spells_t
  {
    mastery_t* potent_afflictions;
    mastery_t* master_demonologist;
    mastery_t* fiery_apocalypse;
  };
  mastery_spells_t mastery_spells;

  std::string dark_intent_target_str;

  // Gains
  gain_t* gains_fel_armor;
  gain_t* gains_felhunter;
  gain_t* gains_life_tap;
  gain_t* gains_soul_leech;
  gain_t* gains_mana_feed;

  // Uptimes
  uptime_t* uptimes_backdraft[ 4 ];

  // Procs
  proc_t* procs_empowered_imp;
  proc_t* procs_impending_doom;
  proc_t* procs_shadow_trance;
  proc_t* procs_ebon_imp;

  // Random Number Generators
  rng_t* rng_soul_leech;
  rng_t* rng_everlasting_affliction;
  rng_t* rng_pandemic;
  rng_t* rng_cremation;
  rng_t* rng_impending_doom;
  rng_t* rng_siphon_life;
  rng_t* rng_ebon_imp;

  // Spells
  spell_t* spells_burning_embers;

  struct glyphs_t
  {
    glyph_t* metamorphosis;
    // Prime
    glyph_t* life_tap;
    glyph_t* shadow_bolt;

    // Major
    glyph_t* chaos_bolt;
    glyph_t* conflagrate;
    glyph_t* corruption;
    glyph_t* bane_of_agony;
    glyph_t* felguard;
    glyph_t* haunt;
    glyph_t* immolate;
    glyph_t* imp;
    glyph_t* incinerate;
    glyph_t* lash_of_pain;
    glyph_t* shadowburn;
    glyph_t* unstable_affliction;
  };
  glyphs_t glyphs;

  double constants_pandemic_gcd;

  int use_pre_soulburn;


  warlock_t( sim_t* sim, const std::string& name, race_type r = RACE_NONE ) : player_t( sim, WARLOCK, name, r )
  {
    tree_type[ WARLOCK_AFFLICTION  ] = TREE_AFFLICTION;
    tree_type[ WARLOCK_DEMONOLOGY  ] = TREE_DEMONOLOGY;
    tree_type[ WARLOCK_DESTRUCTION ] = TREE_DESTRUCTION;

    distance = 40;

    active_pet                  = 0;
    spells_burning_embers       = 0;

    dots_corruption             = get_dot( "corruption" );
    dots_unstable_affliction    = get_dot( "unstable_affliction" );
    dots_bane_of_agony          = get_dot( "bane_of_agony" );
    dots_bane_of_doom           = get_dot( "bane_of_doom" );
    dots_drain_life             = get_dot( "drain_life" );
    dots_drain_soul             = get_dot( "drain_soul" );
    dots_shadowflame            = get_dot( "shadowflame" );
    dots_immolate               = get_dot( "immolate" );
    dots_curse_of_elements      = get_dot( "curse_of_elements" );
    dots_burning_embers         = get_dot( "burning_embers" );

    cooldowns_metamorphosis                   = get_cooldown ( "metamorphosis" );
    cooldowns_infernal                        = get_cooldown ( "summon_infernal" );
    cooldowns_doomguard                       = get_cooldown ( "summon_doomguard" );

    use_pre_soulburn = 0;
  }

  // Character Definition
  virtual void      init_talents();
  virtual void      init_spells();
  virtual void      init_glyphs();
  virtual void      init_race();
  virtual void      init_base();
  virtual void      init_scaling();
  virtual void      init_buffs();
  virtual void      init_gains();
  virtual void      init_uptimes();
  virtual void      init_procs();
  virtual void      init_rng();
  virtual void      init_actions();
  virtual void		  init_resources( bool force );
  virtual void      reset();
  virtual std::vector<talent_translation_t>& get_talent_list();
  virtual std::vector<option_t>& get_options();
  virtual action_t* create_action( const std::string& name, const std::string& options );
  virtual pet_t*    create_pet   ( const std::string& name );
  virtual void      create_pets();
  virtual int       decode_set( item_t& item );
  virtual int       primary_resource() SC_CONST { return RESOURCE_MANA; }
  virtual int       primary_role() SC_CONST     { return ROLE_SPELL; }
  virtual double    composite_spell_power( const school_type school ) SC_CONST;
  virtual double    composite_spell_haste() SC_CONST;
  virtual double    matching_gear_multiplier( const attribute_type attr ) SC_CONST;

  // Event Tracking
  virtual action_expr_t* create_expression( action_t*, const std::string& name );

  // Utilities
  int affliction_effects()
  {
    int effects = 0;
    if ( dots_curse_of_elements -> ticking()    ) effects++;
    if ( dots_bane_of_agony -> ticking()        ) effects++;
    if ( dots_bane_of_doom -> ticking()         ) effects++;
    if ( dots_corruption -> ticking()           ) effects++;
    if ( dots_drain_life -> ticking()           ) effects++;
    if ( dots_drain_soul -> ticking()           ) effects++;
    if ( dots_unstable_affliction -> ticking()  ) effects++;
    if ( buffs_haunted        -> check()        ) effects++;
    if ( buffs_shadow_embrace -> check()        ) effects++;
    return effects;
  }
  int active_dots()
  {
    int dots = 0;
    if ( dots_bane_of_agony -> ticking()            ) dots++;
    if ( dots_bane_of_doom -> ticking()             ) dots++;
    if ( dots_corruption -> ticking()               ) dots++;
    if ( dots_drain_life -> ticking()               ) dots++;
    if ( dots_drain_soul -> ticking()               ) dots++;
    if ( dots_immolate -> ticking()                 ) dots++;
    if ( dots_shadowflame -> ticking()              ) dots++;
    if ( dots_unstable_affliction -> ticking()      ) dots++;
    return dots;
  }
};

// ==========================================================================
// Warlock Pet
// ==========================================================================

struct warlock_pet_t : public pet_t
{
  pet_type_t       pet_type;
  double    damage_modifier;
  attack_t* melee;
  int stats_avaiable;
  int stats2_avaiable;

  gain_t* gains_mana_feed;
  proc_t* procs_mana_feed;

    double get_attribute_base( int level, int stat_type, pet_type_t pet_type )
    {
      double r                      = 0.0;
      const pet_stats::_stat_list_t* base_list = 0;
      const pet_stats::_stat_list_t*  pet_list = 0;


      base_list = pet_stats::pet_base_stats;

        if      ( pet_type == PET_IMP        ) pet_list = pet_stats::imp_base_stats;
        else if ( pet_type == PET_FELGUARD   ) pet_list = pet_stats::felguard_base_stats;
        else if ( pet_type == PET_FELHUNTER  ) pet_list = pet_stats::felhunter_base_stats;
        else if ( pet_type == PET_SUCCUBUS   ) pet_list = pet_stats::succubus_base_stats;
        else if ( pet_type == PET_INFERNAL   ) pet_list = pet_stats::infernal_base_stats;
        else if ( pet_type == PET_DOOMGUARD  ) pet_list = pet_stats::doomguard_base_stats;
        else if ( pet_type == PET_EBON_IMP   ) pet_list = pet_stats::ebon_imp_base_stats;
        else if ( pet_type == PET_VOIDWALKER ) pet_list = pet_stats::voidwalker_base_stats;

      if ( stat_type < 0 || stat_type >= BASE_STAT_MAX )
      {
        return 0.0;
      }

      int i;
      if ( base_list )
      {
       for ( i = 0; base_list[ i ].id != 0; i++ )
        {
          if ( level == base_list[ i ].id )
          {
            r += base_list[ i ].stats[ stat_type ];
            stats_avaiable++;
            break;
          }
        }

      }

      if ( pet_list )
      {
        for ( i = 0; pet_list[ i ].id != 0; i++ )
        {
          if ( level == pet_list[ i ].id )
          {
            r += pet_list[ i ].stats[ stat_type ];
            stats2_avaiable++;
            break;
          }
        }
      }

      return r;
    }

    double get_weapon( int level, pet_type_t pet_type, int n )
       {
         double r                      = 0.0;
         const pet_stats::_weapon_list_t*  weapon_list = 0;

           if      ( pet_type == PET_IMP          ) weapon_list = pet_stats::imp_weapon;
           else if ( pet_type == PET_FELGUARD     ) weapon_list = pet_stats::felguard_weapon;
           else if ( pet_type == PET_FELHUNTER    ) weapon_list = pet_stats::felhunter_weapon;
           else if ( pet_type == PET_SUCCUBUS     ) weapon_list = pet_stats::succubus_weapon;
           else if ( pet_type == PET_INFERNAL     ) weapon_list = pet_stats::infernal_weapon;
           else if ( pet_type == PET_DOOMGUARD    ) weapon_list = pet_stats::doomguard_weapon;
           else if ( pet_type == PET_EBON_IMP     ) weapon_list = pet_stats::ebon_imp_weapon;
           else if ( pet_type == PET_VOIDWALKER   ) weapon_list = pet_stats::voidwalker_weapon;

         if ( weapon_list )
         {
           if ( n == 3 )
           {
             for ( int i = 0; weapon_list[ i ].id != 0; i++ )
             {
               if ( level == weapon_list[ i ].id )
               {
                 r += weapon_list[ i ].swing_time;
                 break;
               }
             }
           }

           else if ( n == 1 )
           {
             for ( int i = 0; weapon_list[ i ].id != 0; i++ )
             {
               if ( level == weapon_list[ i ].id )
               {
                 r += weapon_list[ i ].min_dmg;
                 break;
               }
             }
           }

           else if ( n == 2 )
           {
             for ( int i = 0; weapon_list[ i ].id != 0; i++ )
             {
               if ( level == weapon_list[ i ].id )
               {
                 r += weapon_list[ i ].max_dmg;
                 break;
               }
             }
           }
         }
         if ( n == 3 && r == 0 )r = 1.0; // set swing-time to 1.00 if there is no weapon
         return r;
       }

  warlock_pet_t( sim_t* sim, player_t* owner, const std::string& pet_name, pet_type_t pt, bool guardian = false ) :
    pet_t( sim, owner, pet_name, guardian ), pet_type( pt ), damage_modifier( 1.0 ), melee( 0 )
  {
    gains_mana_feed = get_gain("mana_feed");
    procs_mana_feed = get_proc("mana_feed");
    stats_avaiable = 0;
    stats2_avaiable = 0;

    main_hand_weapon.type       = WEAPON_BEAST;
    main_hand_weapon.min_dmg    = get_weapon( level, pet_type, 1 );
    main_hand_weapon.max_dmg    = get_weapon( level, pet_type, 2 );
    main_hand_weapon.damage     = ( main_hand_weapon.min_dmg + main_hand_weapon.max_dmg ) / 2;
    main_hand_weapon.swing_time = get_weapon( level, pet_type, 3 );
    if (main_hand_weapon.swing_time == 0 )
    {
      sim -> errorf( "Pet %s has swingtime == 0.\n", name());
      assert(0);
    }
  }


  virtual bool ooc_buffs() { return true; }

  virtual void init_base()
  {
    pet_t::init_base();

    attribute_base[ ATTR_STRENGTH  ]  = get_attribute_base( level, BASE_STAT_STRENGTH, pet_type );
    attribute_base[ ATTR_AGILITY   ]  = get_attribute_base( level, BASE_STAT_AGILITY, pet_type );
    attribute_base[ ATTR_STAMINA   ]  = get_attribute_base( level, BASE_STAT_STAMINA, pet_type );
    attribute_base[ ATTR_INTELLECT ]  = get_attribute_base( level, BASE_STAT_INTELLECT, pet_type );
    attribute_base[ ATTR_SPIRIT    ]  = get_attribute_base( level, BASE_STAT_SPIRIT, pet_type );
    resource_base[ RESOURCE_HEALTH ]  = get_attribute_base( level, BASE_STAT_HEALTH, pet_type );
    resource_base[ RESOURCE_MANA   ]  = get_attribute_base( level, BASE_STAT_MANA, pet_type );
    initial_attack_crit_per_agility   = get_attribute_base( level, BASE_STAT_MELEE_CRIT_PER_AGI, pet_type );
    initial_spell_crit_per_intellect  = get_attribute_base( level, BASE_STAT_SPELL_CRIT_PER_INT, pet_type );
    initial_dodge_per_agility         = get_attribute_base( level, BASE_STAT_DODGE_PER_AGI, pet_type );
    base_spell_crit                   = get_attribute_base( level, BASE_STAT_SPELL_CRIT, pet_type );
    base_attack_crit                  = get_attribute_base( level, BASE_STAT_MELEE_CRIT, pet_type );
    base_mp5                          = get_attribute_base( level, BASE_STAT_MP5, pet_type );


    if ( stats_avaiable != 13 )
      sim -> errorf( "Pet %s has no general base stats avaiable on level=%.i.\n", name(), level);
    if ( stats2_avaiable != 13 )
      sim -> errorf( "Pet %s has no base stats avaiable on level=%.i.\n", name(), level);

    initial_attack_power_per_strength = 1.0; // tested
    base_attack_power = -20; // technically, the first 20 str give 0 ap. - tested
    stamina_per_owner = 0.6496; // level invariant, tested
    intellect_per_owner = 0; // removed in cata, tested

    base_attack_crit                  += 0.0328; // seems to be level invariant, untested
    base_spell_crit                   += 0.0092; // seems to be level invariant, untested
    initial_attack_crit_per_agility   += 0.01 / 52.0; // untested
    initial_spell_crit_per_intellect  += owner -> initial_spell_crit_per_intellect; // untested
    health_per_stamina = 10.0; // untested!
    mana_per_intellect = 0; // tested - does not scale with pet int, but with owner int, at level/80 * 7.5 mana per point of owner int that exceeds owner base int
    mp5_per_intellect  = 2.0 / 3.0; // untested!


  }

  virtual void init_resources( bool force )
  {
    bool mana_force = ( force || resource_initial[ RESOURCE_MANA ] == 0 );
    player_t::init_resources( force );
    if ( mana_force ) resource_initial[ RESOURCE_MANA ] += ( owner -> intellect() - owner -> attribute_base[ ATTR_INTELLECT ] ) * (level / 80) * 7.5;
    resource_current[ RESOURCE_MANA ] = resource_max[ RESOURCE_MANA ] = resource_initial[ RESOURCE_MANA ];
  }

  virtual void schedule_ready( double delta_time=0,
                               bool   waiting=false )
  {
    if ( melee && ! melee -> execute_event )
    {
      melee -> schedule_execute();
    }

    pet_t::schedule_ready( delta_time, waiting );
  }

  virtual void summon( double duration=0 )
  {
    warlock_t*  o = owner -> cast_warlock();
    pet_t::summon( duration );
    if ( o -> talent_demonic_pact -> rank() )
      sim -> auras.demonic_pact -> trigger();
  }

  virtual void dismiss()
  {
    pet_t::dismiss();
	/* Commenting this out for now - we never dismiss the real pet during combat
	anyway, and we don't want to accidentally turn off DP when guardians are dismissed
    warlock_t*  o = owner -> cast_warlock();
    if ( o -> talent_demonic_pact -> rank() )
      sim -> auras.demonic_pact -> expire();
	  */
  }

  virtual void interrupt()
  {
    pet_t::interrupt();
    if ( melee ) melee -> cancel();
  }

  virtual double composite_spell_haste() SC_CONST
  {
    double h = player_t::composite_spell_haste();
    h *= owner -> spell_haste;
    return h;
  }

  virtual double composite_attack_haste() SC_CONST
  {
    double h = player_t::composite_attack_haste();
    h *= owner -> spell_haste;
    return h;
  }

  virtual double composite_spell_power( const school_type school ) SC_CONST
  {
    double sp = pet_t::composite_spell_power( school );
    sp += owner -> composite_spell_power( school ) * ( level / 80 ) * 0.5 * owner -> composite_spell_power_multiplier();
    return sp;
  }

  virtual double composite_attack_power() SC_CONST
  {
    double ap = pet_t::composite_attack_power();
    ap += owner -> composite_spell_power( SCHOOL_MAX ) * ( level / 80 ) * owner -> composite_spell_power_multiplier();
    return ap;
  }

  virtual double composite_attack_crit() SC_CONST
  {
    return owner -> composite_spell_crit(); // Seems to just use our crit directly, based on very rough numbers, needs more testing.
  }

  virtual double composite_spell_crit() SC_CONST
  {
    return owner -> composite_spell_crit(); // Seems to just use our crit directly, based on very rough numbers, needs more testing.
  }
};

// ==========================================================================
// Warlock Main Pet
// ==========================================================================

struct warlock_main_pet_t : public warlock_pet_t
{
  warlock_main_pet_t( sim_t* sim, player_t* owner, const std::string& pet_name, pet_type_t pt ) :
    warlock_pet_t( sim, owner, pet_name, pt )
  {}

  virtual void summon( double duration=0 )
  {
    warlock_t* o = owner -> cast_warlock();
    o -> active_pet = this;
    warlock_pet_t::summon( duration );
  }

  virtual void dismiss()
  {
    warlock_t* o = owner -> cast_warlock();
    warlock_pet_t::dismiss();
    o -> active_pet = 0;
  }

  virtual double composite_attack_expertise() SC_CONST
  {
    return owner -> spell_hit * 26.0 / 17.0;
  }

  virtual int primary_resource() SC_CONST { return RESOURCE_MANA; }

  virtual double composite_player_multiplier( const school_type school ) SC_CONST
  {
    double m = player_t::composite_player_multiplier( school );

    warlock_t* o = owner -> cast_warlock();

    if ( o -> race == RACE_ORC )
    {
      m  *= 1.05;
    }

    m *= 1.0 + ( o -> mastery_spells.master_demonologist -> ok() * o -> composite_mastery() * o -> mastery_spells.master_demonologist -> effect_base_value( 3 ) / 10000.0 );

    return m;
  }

  virtual double composite_mp5() SC_CONST
  {
    double h = player_t::composite_mp5();
    h += mp5_per_intellect * owner -> intellect();
    return h;
  }

};

// ==========================================================================
// Warlock Guardian Pet
// ==========================================================================

struct warlock_guardian_pet_t : public warlock_pet_t
{
  warlock_guardian_pet_t( sim_t* sim, player_t* owner, const std::string& pet_name, pet_type_t pt ) :
    warlock_pet_t( sim, owner, pet_name, pt, true )
  {}

  virtual void summon( double duration=0 )
  {
    warlock_pet_t::summon( duration );
    reset();
  }

  virtual double composite_attack_hit() SC_CONST
    { return 0; }

  virtual double composite_attack_expertise() SC_CONST
    { return 0; }
};

namespace { // ANONYMOUS NAMESPACE ==========================================

// ==========================================================================
// Warlock Spell
// ==========================================================================

struct warlock_spell_t : public spell_t
{
  bool usable_pre_combat;

  void _init_warlock_spell_t()
  {
    may_crit      = true;
    tick_may_crit = true;
    dot_behavior  = DOT_REFRESH;
    weapon_multiplier = 0.0;
    base_crit_multiplier = 1.33;
  }

  warlock_spell_t( const char* n, player_t* player, const school_type s, int t ) :
      spell_t( n, player, RESOURCE_MANA, s, t ), usable_pre_combat( false)
  {
    _init_warlock_spell_t();
  }

  warlock_spell_t( const active_spell_t& s, int t = TREE_NONE ) :
      spell_t( s, t ), usable_pre_combat( false)
  {
    _init_warlock_spell_t();
  }

  warlock_spell_t( const char* n, player_t* player, const char* sname, int t = TREE_NONE ) :
      spell_t( n, sname, player, t ), usable_pre_combat( false)
  {
    _init_warlock_spell_t();
  }

  warlock_spell_t( const char* n, player_t* player, const uint32_t id, int t = TREE_NONE ) :
      spell_t( n, id, player, t ), usable_pre_combat( false)
  {
    _init_warlock_spell_t();
  }

  // ==========================================================================
  // Warlock Spell
  // ==========================================================================

  // warlock_spell_t::haste ===================================================

  virtual double haste() SC_CONST
  {
    warlock_t* p = player -> cast_warlock();
    double h = spell_t::haste();

    if ( p -> buffs_eradication -> up() )
    {
      h *= 1.0 / ( 1.0 + p -> buffs_eradication -> effect_base_value( 1 ) / 100.0 );
    }


    if ( p -> buffs_demon_soul_felguard -> up() )
    {
      h *= 1.0 / ( 1.0 + p -> buffs_demon_soul_felguard -> effect_base_value( 1 ) / 100.0 );
    }

    return h;
  }

  // warlock_spell_t::player_buff =============================================

  virtual void player_buff()
  {
    warlock_t* p = player -> cast_warlock();

    spell_t::player_buff();

    if ( p -> buffs_metamorphosis -> up() )
    {
      player_multiplier *= 1.0 + p -> buffs_metamorphosis -> effect_base_value( 3 ) / 100.0 + ( p -> mastery_spells.master_demonologist -> ok() * p -> composite_mastery() * p -> mastery_spells.master_demonologist -> effect_base_value( 3 ) / 10000.0 );
    }

    player_multiplier *= 1.0 + ( p -> talent_demonic_pact -> effect_base_value( 3 ) / 100.0 );

	  if ( p -> buffs_tier10_4pc_caster -> up() )
	  {
      player_multiplier *= ( 1.0 + p -> buffs_tier10_4pc_caster -> effect_base_value( 1 ) / 100.0 );
	  }

    if ( p -> buffs_demon_soul_felguard -> up() && ( school == SCHOOL_FIRE || school == SCHOOL_SHADOW ) )
    {
	    player_multiplier *= 1.0 + p -> buffs_demon_soul_felguard -> effect_base_value( 2 ) / 100.0;
    }

    if ( p -> buffs_demon_soul_imp -> up() && execute_time() > 0 && s_tree == WARLOCK_DESTRUCTION )
    {
      player_crit_multiplier *= 1.0 + p -> buffs_demon_soul_imp -> effect_base_value( 1 ) / 100.0;
    }

    double fire_multiplier=1.0;
    double shadow_multiplier=1.0;

    // Fire
    fire_multiplier *= 1.0 + ( p -> passive_spells.cataclysm -> effect_base_value( 1 ) / 100.0 );
    if ( p -> mastery_spells.fiery_apocalypse -> ok() )
      fire_multiplier *= 1.0 + ( p -> mastery_spells.fiery_apocalypse -> ok() * p -> composite_mastery() * p -> mastery_spells.fiery_apocalypse -> effect_base_value( 2 ) / 10000.0 );
    fire_multiplier *= 1.0 +  p -> passive_spells.demonic_knowledge -> effect_base_value( 1 ) / 100.0 ;

    // Shadow
    shadow_multiplier *= 1.0 + ( p -> passive_spells.demonic_knowledge -> effect_base_value( 1 ) / 100.0 );
    shadow_multiplier *= 1.0 + ( p -> passive_spells.shadow_mastery -> effect_base_value( 1 ) / 100.0 );
    shadow_multiplier *= 1.0 + trigger_deaths_embrace( this );

    if ( school == SCHOOL_FIRE )
      player_multiplier *= fire_multiplier;
    if ( school == SCHOOL_SHADOW )
      player_multiplier *= shadow_multiplier;
    if ( school == SCHOOL_SHADOWFLAME )
    {
      if ( fire_multiplier > shadow_multiplier )
        player_multiplier *= fire_multiplier;
      else
        player_multiplier *= shadow_multiplier;
    }

  }

  // warlock_spell_t::target_debuff ============================================

  virtual void target_debuff( int dmg_type )
  {
    warlock_t* p = player -> cast_warlock();

    spell_t::target_debuff( dmg_type );

    if ( p -> buffs_bane_of_havoc -> up() )
      target_multiplier *= ( 1.0 + p -> buffs_bane_of_havoc -> effect_base_value( 1 ) / 100.0 );

  }

  virtual double total_td_multiplier() SC_CONST
  {
    double shadow_td_multiplier = 1.0;
    warlock_t* p = player -> cast_warlock();

    if ( school == SCHOOL_SHADOW || school == SCHOOL_SHADOWFLAME )
    {
      // Shadow TD
      if ( p -> mastery_spells.potent_afflictions -> ok() )
      {
        shadow_td_multiplier *= 1.0 + ( p -> mastery_spells.potent_afflictions -> ok() *  p -> composite_mastery()  * p -> mastery_spells.potent_afflictions -> effect_base_value( 2 ) / 10000.0 );
      }
      if ( p -> buffs_shadow_embrace -> up() )
      {
        shadow_td_multiplier *= 1.0 + p -> buffs_shadow_embrace ->  check() * p -> buffs_shadow_embrace -> effect_base_value( 1 ) / 100.0;
      }
      if ( p -> buffs_haunted -> up() )
      {
        shadow_td_multiplier *= 1.0 + p -> buffs_haunted -> effect_base_value( 3 ) / 100.0 + ( p -> glyphs.haunt -> effect_base_value( 1 ) / 100.0 );
      }
      if ( p -> buffs_demon_soul_felhunter -> up() )
      {
        shadow_td_multiplier *=  1.0 + p -> buffs_demon_soul_felhunter -> effect_base_value( 1 ) / 100.0;
      }
    }

    return spell_t::total_td_multiplier() * shadow_td_multiplier;
  }

  // warlock_spell_t::execute ==================================================

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    spell_t::execute();
    if ( p -> buffs_demon_soul_imp -> up() && execute_time() > 0 && s_tree == WARLOCK_DESTRUCTION )
    {
      p -> buffs_demon_soul_imp -> decrement();
    }

  }

  virtual double gcd() SC_CONST
  {
    double t = spell_t::gcd();

    if ( usable_pre_combat && ! player -> in_combat ) t=0;

    return t;
  }

  virtual double execute_time() SC_CONST
  {
     double t = spell_t::execute_time();

     if ( usable_pre_combat && !player -> in_combat ) t=0;

     return t;
  }


  // trigger_impending_doom ===================================================

  static void trigger_impending_doom ( spell_t* s )
  {
    warlock_t* p = s -> player -> cast_warlock();
    if ( p -> talent_impending_doom -> rank() )
    {
      if ( p -> rng_impending_doom -> roll ( p -> talent_impending_doom -> rank() ? p -> talent_impending_doom -> proc_chance()  : 0 ) )
      {
        p -> procs_impending_doom -> occur();
        if ( p -> cooldowns_metamorphosis -> remains() > p -> talent_impending_doom -> effect_base_value( 2 ) )
          p -> cooldowns_metamorphosis -> ready -= p -> talent_impending_doom -> effect_base_value( 2 );
        else
          p -> cooldowns_metamorphosis -> reset();
      }
    }
  }

  // trigger_soul_leech =======================================================

  static void trigger_soul_leech( spell_t* s )
  {
    warlock_t* p = s -> player -> cast_warlock();

    if ( p -> talent_soul_leech -> rank() )
    {
      p -> resource_gain( RESOURCE_HEALTH, p -> resource_max[ RESOURCE_HEALTH ] * p -> talent_soul_leech -> effect_base_value( 1 ) / 100.0, p ->gains_soul_leech );
      p -> resource_gain( RESOURCE_MANA, p -> resource_max[ RESOURCE_MANA ] * p -> talent_soul_leech -> effect_base_value ( 1 ) / 100.0, p -> gains_soul_leech );

      p -> trigger_replenishment();
    }
  }

  // trigger_decimation ======================================================

  static void trigger_decimation( warlock_spell_t* s, int result)
  {
    warlock_t* p = s -> player -> cast_warlock();
    if ( ( result !=  RESULT_HIT ) && ( result != RESULT_CRIT ) ) return;
    if ( s -> target -> health_percentage() > p -> talent_decimation -> effect_base_value( 2 ) ) return;
    p -> buffs_decimation -> trigger();
  }

  // trigger_deaths_embrace ===================================================

  static double trigger_deaths_embrace( spell_t* s )
  {
    warlock_t* p = s -> player -> cast_warlock();

    if ( ! p -> talent_deaths_embrace -> rank() ) return 0;

    if ( s -> target -> health_percentage() < p -> talent_deaths_embrace -> rank_spell() -> effect_base_value( 3 ) )
    {
    	return p -> talent_deaths_embrace -> effect_base_value( 2 ) / 100.0;
    }

    return 0;
  }

  // trigger_everlasting_affliction ===========================================

  static void trigger_everlasting_affliction( spell_t* s )
  {
    warlock_t* p = s -> player -> cast_warlock();

    if ( ! p -> talent_everlasting_affliction -> rank() ) return;

    if ( ! p -> dots_corruption -> ticking() ) return;

    if ( p -> talent_everlasting_affliction -> rank() )
    {
      if ( p -> rng_everlasting_affliction -> roll( p -> talent_everlasting_affliction -> proc_chance() ) )
      {
        p -> dots_corruption -> action -> refresh_duration();
      }
    }
  }
};

// ==========================================================================
// Warlock Pet Melee
// ==========================================================================

struct warlock_pet_melee_t : public attack_t
{
  warlock_pet_melee_t( player_t* player, const char* name ) :
    attack_t( name, player, RESOURCE_NONE, SCHOOL_PHYSICAL, TREE_NONE, false )
  {
    warlock_pet_t* p = ( warlock_pet_t* ) player -> cast_pet();

    weapon = &( p -> main_hand_weapon );
    base_execute_time = weapon -> swing_time;
    may_crit    = true;
    background  = true;
    repeating   = true;

    base_multiplier *= p -> damage_modifier;
  }

  virtual void player_buff()
  {
    warlock_pet_t* p = ( warlock_pet_t* ) player -> cast_pet();
    warlock_t* o = p -> owner -> cast_warlock();
    attack_t::player_buff();

    if ( o -> buffs_hand_of_guldan -> up() )
    {
      player_crit += 0.10;
    }

    if ( o -> buffs_tier10_4pc_caster -> up() )
    {
      player_multiplier *= 1.0 + o -> buffs_tier10_4pc_caster -> effect_base_value( 1 ) / 100.0;
    }

  }
};

// ==========================================================================
// Warlock Pet Attack
// ==========================================================================

struct warlock_pet_attack_t : public attack_t
{
  warlock_pet_attack_t( const char* n, player_t* player, int r=RESOURCE_MANA, const school_type s=SCHOOL_PHYSICAL ) :
    attack_t( n, player, r, s, TREE_NONE, true )
  {
    warlock_pet_t* p = ( warlock_pet_t* ) player -> cast_pet();

    weapon = &( p -> main_hand_weapon );
    may_crit   = true;
    special = true;
  }

  warlock_pet_attack_t( const char* n, player_t* player, const char* sname, int t = TREE_NONE ) :
    attack_t( n, sname, player, t, true )
  {
    may_crit   = true;
    special = true;
  }
  warlock_pet_attack_t( const char* n, const uint32_t id, player_t* player, int t = TREE_NONE ) :
      attack_t( n, id, player, t, true )
  {
    may_crit   = true;
    special = true;
  }

  virtual void player_buff()
  {
    warlock_pet_t* p = ( warlock_pet_t* ) player -> cast_pet();
    warlock_t* o = p -> owner -> cast_warlock();
    attack_t::player_buff();

    if ( o -> buffs_hand_of_guldan -> up() )
    {
      player_crit += 0.10;
    }

    if ( o -> buffs_tier10_4pc_caster -> up() )
    {
      player_multiplier *= 1.0 + o -> buffs_tier10_4pc_caster -> effect_base_value( 1 ) / 100.0;
    }

  }
};

// trigger_mana_feed =======================================================

static void trigger_mana_feed( action_t* s, double travel_result )
{
  warlock_pet_t* a = ( warlock_pet_t* ) s -> player -> cast_pet();
  warlock_t* p = a -> owner -> cast_warlock();
  if ( p -> talent_mana_feed -> rank() )
  {
    if ( travel_result == RESULT_CRIT )
    {
      p -> resource_gain( RESOURCE_MANA, p -> resource_max[ RESOURCE_MANA ] / 100.0 * p -> talent_mana_feed -> effect_base_value( 3 ), p -> gains_mana_feed );
      a -> procs_mana_feed -> occur();
    }
  }
}

// ==========================================================================
// Warlock Pet Spell
// ==========================================================================

struct warlock_pet_spell_t : public spell_t
{

  warlock_pet_spell_t( const char* n, player_t* player, int r=RESOURCE_MANA, const school_type s=SCHOOL_SHADOW ) :
    spell_t( n, player, r, s )
  {
    may_crit          = true;
  }

  warlock_pet_spell_t( const active_spell_t& s, int t = TREE_NONE ) :
    spell_t( s, t )
  {
    may_crit          = true;
  }

  warlock_pet_spell_t( const char* n, player_t* player, const char* sname, int t = TREE_NONE ) :
      spell_t( n, sname, player, t )
  {
    may_crit          = true;
  }

  warlock_pet_spell_t( const char* n, const uint32_t id, player_t* player, int t = TREE_NONE ) :
      spell_t( n, id, player, t )
  {
    may_crit          = true;
  }

  virtual void player_buff()
  {
    warlock_pet_t* p = ( warlock_pet_t* ) player -> cast_pet();
    warlock_t* o = p -> owner -> cast_warlock();
    spell_t::player_buff();

    if ( o -> buffs_hand_of_guldan -> up() )
    {
      player_crit += 0.10;
    }

    if ( o -> buffs_tier10_4pc_caster -> up() )
    {
      player_multiplier *= 1.0 + o -> buffs_tier10_4pc_caster -> effect_base_value( 1 ) / 100.0;
    }

  }
};

// ==========================================================================
// Pet Imp
// ==========================================================================

struct imp_pet_t : public warlock_main_pet_t
{
  struct firebolt_t : public warlock_pet_spell_t
  {
    firebolt_t( player_t* player ):
      warlock_pet_spell_t( "firebolt", player, "Firebolt" )
    {
      warlock_t*  o = player -> cast_pet() -> owner -> cast_warlock();
      base_multiplier *= 1.0 + ( o -> glyphs.imp -> base_value() );
      direct_power_mod = 0.619; // seems to be 0.00 in DBC now?
      base_execute_time += o -> talent_dark_arts -> effect_base_value( 1 ) / 1000.0;
      if ( o -> bugs ) min_gcd = 1.5;
    }

    virtual void travel( int travel_result, double travel_dmg);
  };

  imp_pet_t( sim_t* sim, player_t* owner ) :
    warlock_main_pet_t( sim, owner, "imp", PET_IMP )
  {
    action_list_str += "/snapshot_stats";
    action_list_str += "/firebolt";
  }

  virtual void init_base()
  {
    warlock_main_pet_t::init_base();

    // untested !!
    mana_per_intellect = 14.28;
    mp5_per_intellect  = 5.0 / 6.0;
    // untested !!
  }

  virtual action_t* create_action( const std::string& name,
                                   const std::string& options_str )
  {
    if ( name == "firebolt" ) return new firebolt_t( this );

    return warlock_main_pet_t::create_action( name, options_str );
  }
};

// ==========================================================================
// Pet Felguard
// ==========================================================================

struct felguard_pet_t : public warlock_main_pet_t
{
  struct legion_strike_t : public warlock_pet_attack_t
  {
    legion_strike_t( player_t* player ) :
		warlock_pet_attack_t( "legion_strike", player, "Legion Strike" )
    {
      felguard_pet_t* p = ( felguard_pet_t* ) player -> cast_pet();
      warlock_t*      o = p -> owner -> cast_warlock();
      aoe               = true;
      direct_power_mod  = 0.264;
      base_spell_power_multiplier = 1.0;
      base_attack_power_multiplier = 0.0;

      weapon   = &( p -> main_hand_weapon );
      base_multiplier *= 1.0 + o -> talent_dark_arts -> effect_base_value( 2 ) / 100.0;
      if ( o -> glyphs.felguard -> ok() ) base_multiplier *= 1.0 + o -> glyphs.felguard -> base_value();
    }

    virtual void execute()
    {
      warlock_pet_attack_t::execute();
      trigger_mana_feed ( this, result );
    }
  };

  struct felstorm_tick_t : public warlock_pet_attack_t
  {
    felstorm_tick_t( player_t* player ) :
        warlock_pet_attack_t( "felstorm_tick", 89753, player )
    {
      direct_power_mod = 0.33; // hardcoded from the tooltip
      dual        = true;
      background  = true;
      aoe         = true;
      direct_tick = true;
      stats       = player -> get_stats( "felstorm" );
    }

    virtual void execute()
    {
      warlock_pet_attack_t::execute();
      tick_dmg = direct_dmg;
      update_stats( DMG_OVER_TIME );
    }
  };

  struct felstorm_t : public warlock_pet_attack_t
  {
    felstorm_tick_t* felstorm_tick;

    felstorm_t( player_t* player ) :
      warlock_pet_attack_t( "felstorm", 89751, player )
    {
      felguard_pet_t* p = ( felguard_pet_t* ) player -> cast_pet();
      aoe       = true;
      harmful   = false;
      tick_zero = true;

      felstorm_tick = new felstorm_tick_t( p );
    }

    virtual void tick()
    {
      if ( sim -> debug ) log_t::output( sim, "%s ticks (%d of %d)", name(), current_tick, num_ticks );

      felstorm_tick -> weapon = &( player -> main_hand_weapon );
      felstorm_tick -> execute();

      update_time( DMG_OVER_TIME );
    }
  };

  struct melee_t : public warlock_pet_melee_t
  {
    melee_t( player_t* player ) :
        warlock_pet_melee_t( player, "melee" )
    { }
  };

  felguard_pet_t( sim_t* sim, player_t* owner ) :
      warlock_main_pet_t( sim, owner, "felguard", PET_FELGUARD )
  {
    damage_modifier = 1.0;

    action_list_str += "/snapshot_stats";
    action_list_str += "/felstorm";
    action_list_str += "/legion_strike";
    action_list_str += "/wait_until_ready";
  }

  virtual void init_base()
  {
    warlock_main_pet_t::init_base();

    melee = new melee_t( this );
  }

  virtual action_t* create_action( const std::string& name,
                                   const std::string& options_str )
  {
    if ( name == "legion_strike"   ) return new legion_strike_t( this );
    if ( name == "felstorm"        ) return new      felstorm_t( this );

    return warlock_main_pet_t::create_action( name, options_str );
  }
};

// ==========================================================================
// Pet Felhunter
// ==========================================================================

struct felhunter_pet_t : public warlock_main_pet_t
{
   // TODO: Need to add fel intelligence on the warlock while felhunter is out
  struct shadow_bite_t : public warlock_pet_spell_t
  {
    shadow_bite_t( player_t* player ) :
      warlock_pet_spell_t( "shadow_bite", player, "Shadow Bite" )
    {
      felhunter_pet_t* p = ( felhunter_pet_t* ) player -> cast_pet();
      warlock_t*       o = p -> owner -> cast_warlock();
      base_multiplier *= 1.0 + o -> talent_dark_arts -> effect_base_value( 3 ) / 100.0;
      direct_power_mod   = 0.614; // from tooltip - assuming the 0.5 factor is not used, like for lash of pain and torment
    }

    virtual void player_buff()
    {
      warlock_pet_spell_t::player_buff();

      warlock_t*  o = player -> cast_pet() -> owner -> cast_warlock();
      player_multiplier *= 1.0 + o -> active_dots() * effect_base_value( 3 ) / 100.0;
    }

    virtual void travel( int travel_result, double travel_dmg)
    {
      warlock_pet_spell_t::travel(travel_result, travel_dmg);
      trigger_mana_feed ( this, travel_result );
    }
  };

  felhunter_pet_t( sim_t* sim, player_t* owner ) :
      warlock_main_pet_t( sim, owner, "felhunter", PET_FELHUNTER )
  {
    damage_modifier = 0.8;

    action_list_str += "/snapshot_stats";
    action_list_str += "/shadow_bite";
    action_list_str += "/wait_until_ready";
  }

  virtual void init_base()
  {
    warlock_pet_t::init_base();

    // untested in cataclyms!!
    health_per_stamina = 9.5;
    mana_per_intellect = 11.55;
    mp5_per_intellect  = 8.0 / 324.0;
    base_mp5 = 11.22;
    // untested in cataclyms!!

    melee = new warlock_pet_melee_t( this, "felhunter_melee" );
  }

  virtual action_t* create_action( const std::string& name,
                                   const std::string& options_str )
  {
    if ( name == "shadow_bite" ) return new shadow_bite_t( this );

    return warlock_main_pet_t::create_action( name, options_str );
  }

  virtual void summon( double duration=0 )
  {
    sim -> auras.fel_intelligence -> trigger();
    warlock_main_pet_t::summon( duration );
  }

  virtual void dismiss()
  {
    warlock_main_pet_t::dismiss();
    sim -> auras.fel_intelligence -> expire();
  }
};

// ==========================================================================
// Pet Succubus
// ==========================================================================

struct succubus_pet_t : public warlock_main_pet_t
{
  struct lash_of_pain_t : public warlock_pet_spell_t
  {
    lash_of_pain_t( player_t* player ) :
        warlock_pet_spell_t( "lash_of_pain", player, "Lash of Pain" )
    {
      warlock_t*  o     = player -> cast_pet() -> owner -> cast_warlock();
      base_multiplier  *= 1.0 + ( o -> glyphs.lash_of_pain -> base_value() );
      direct_power_mod  = 0.612; // from the tooltip - tests show the 0.5 factor is not used
      if ( o -> bugs ) min_gcd = 1.5;
    }

    virtual void travel( int travel_result, double travel_dmg)
    {
      warlock_pet_spell_t::travel(travel_result, travel_dmg);
      trigger_mana_feed ( this, travel_result );
    }
  };

  succubus_pet_t( sim_t* sim, player_t* owner ) :
      warlock_main_pet_t( sim, owner, "succubus", PET_SUCCUBUS )
  {
    damage_modifier = 1.025;

    action_list_str += "/snapshot_stats";
    action_list_str += "/lash_of_pain";
  }

  virtual action_t* create_action( const std::string& name,
                                   const std::string& options_str )
  {
    if ( name == "lash_of_pain" ) return new lash_of_pain_t( this );

    return warlock_main_pet_t::create_action( name, options_str );
  }

  virtual double composite_spell_haste() SC_CONST
   {
     double h = player_t::composite_spell_haste();
     return h;
   }

   virtual double composite_attack_haste() SC_CONST
   {
     double h = player_t::composite_attack_haste();
     return h;
   }
};

// ==========================================================================
// Pet Voidwalker
// ==========================================================================

struct voidwalker_pet_t : public warlock_main_pet_t
{
  struct torment_t : public warlock_pet_spell_t
  {
    torment_t( player_t* player ) :
        warlock_pet_spell_t( "torment", player, "Torment" )
    {
      direct_power_mod = 0.512;
    }

    virtual void travel( int travel_result, double travel_dmg)
    {
      warlock_pet_spell_t::travel(travel_result, travel_dmg);
      trigger_mana_feed ( this, travel_result ); // untested
    }
  };

  voidwalker_pet_t( sim_t* sim, player_t* owner ) :
      warlock_main_pet_t( sim, owner, "voidwalker", PET_VOIDWALKER )
  {
    damage_modifier = 0.86;

    action_list_str += "/snapshot_stats";
    action_list_str += "/torment";
  }

  virtual void init_base()
  {
    warlock_main_pet_t::init_base();

    melee = new warlock_pet_melee_t( this, "voidwalker_melee" );
  }

  virtual action_t* create_action( const std::string& name,
                                   const std::string& options_str )
  {
    if ( name == "torment" ) return new torment_t( this );

    return warlock_main_pet_t::create_action( name, options_str );
  }
};
// ==========================================================================
// Pet Infernal
// ==========================================================================

struct infernal_pet_t : public warlock_guardian_pet_t
{

  // Immolation Damage Spell =====================================================

  struct immolation_damage_t : public warlock_pet_spell_t
  {
    immolation_damage_t( player_t* player ) :
      warlock_pet_spell_t( "immolation_dmg", 20153, player )
    {
      dual        = true;
      background  = true;
      aoe         = true;
      direct_tick = true;
      stats = player -> get_stats( "infernal_immolation" );
      direct_power_mod  = 0.4;
    }

    virtual void execute()
    {
      warlock_pet_spell_t::execute();
      tick_dmg = direct_dmg;
      update_stats( DMG_OVER_TIME );
    }
  };

  struct infernal_immolation_t : public warlock_pet_spell_t
  {
    immolation_damage_t* immolation_damage;

    infernal_immolation_t( player_t* player, const std::string& options_str ) :
      warlock_pet_spell_t( "infernal_immolation", 19483, player )
    {
      parse_options( NULL, options_str );

      harmful           = false;
      num_ticks         = 1;
      number_ticks      = 1;
      scale_with_haste  = false;

      warlock_pet_t* p = ( warlock_pet_t* ) player -> cast_pet();
      immolation_damage = new immolation_damage_t( p );
    }

  virtual void tick()
  {
    current_tick = 0; // ticks indefinitely
    immolation_damage -> execute();
    update_time( DMG_OVER_TIME );
  }
};


  infernal_pet_t( sim_t* sim, player_t* owner ) :
      warlock_guardian_pet_t( sim, owner, "infernal", PET_INFERNAL )
  {
    action_list_str += "/snapshot_stats";
    if ( level >= 50) action_list_str += "/immolation,if=!ticking";
  }

  virtual void init_base()
  {
    warlock_guardian_pet_t::init_base();

    melee      = new   warlock_pet_melee_t( this, "Infernal Melee" );
  }

  virtual action_t* create_action( const std::string& name,
                                   const std::string& options_str )
  {
    if ( name == "immolation" ) return new infernal_immolation_t( this, options_str );

    return warlock_guardian_pet_t::create_action( name, options_str );
  }
};

// ==========================================================================
// Pet Doomguard
// ==========================================================================

struct doomguard_pet_t : public warlock_guardian_pet_t
{
  struct doom_bolt_t : public warlock_pet_spell_t
  {
    doom_bolt_t( player_t* player ) :
      warlock_pet_spell_t( "doombolt", player, "Doom Bolt" )
    {
      base_dd_min *= 1.333; // Based on testing 2010/11/20
      base_dd_max *= 1.333; // Based on testing 2010/11/20
      direct_power_mod = 0.95; // Based on testing 2010/11/20
      base_execute_time = 2.5;
    }
  };

  doomguard_pet_t( sim_t* sim, player_t* owner ) :
      warlock_guardian_pet_t( sim, owner, "doomguard", PET_DOOMGUARD )
  { }

  virtual void init_base()
  {
    warlock_guardian_pet_t::init_base();

    action_list_str += "/snapshot_stats";
    action_list_str += "/doom_bolt";
  }

  virtual action_t* create_action( const std::string& name,
                                   const std::string& options_str )
  {
    if ( name == "doom_bolt" ) return new doom_bolt_t( this );

    return warlock_guardian_pet_t::create_action( name, options_str );
  }
};

// ==========================================================================
// Pet Ebon Imp
// ==========================================================================

struct ebon_imp_pet_t : public warlock_guardian_pet_t
{
  ebon_imp_pet_t( sim_t* sim, player_t* owner ) :
    warlock_guardian_pet_t( sim, owner, "ebon_imp", PET_EBON_IMP )
  {
    action_list_str += "/snapshot_stats";
    action_list_str += "/wait_until_ready";
  }

  virtual double composite_attack_power() SC_CONST
  {
    return 0;
  }

  virtual void init_base()
  {
    warlock_guardian_pet_t::init_base();

    melee = new warlock_pet_melee_t( this, "ebon_imp_melee" );
  }
};


// Curse of Elements Debuff ==================================================

struct coe_debuff_t : public debuff_t
{
  coe_debuff_t( target_t* t ) : debuff_t( t, "curse_of_elements", 1, 300.0 )
    {}

  virtual void expire()
  {
    if( player )
    {
      player = 0;
    }
    debuff_t::expire();
  }
};

// Curse of Elements Spell ===================================================

struct curse_of_elements_t : public warlock_spell_t
{
  curse_of_elements_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "curse_of_the_elements", player, "Curse of the Elements" )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    trigger_gcd -= p -> constants_pandemic_gcd * p -> talent_pandemic -> rank();
  }

  virtual void execute()
  {
    warlock_spell_t::execute();
    if ( result_is_hit() )
    {
      warlock_t* p = player -> cast_warlock();
      target -> debuffs.curse_of_elements -> expire();
      target -> debuffs.curse_of_elements -> trigger( 1, effect_base_value( 2 ) );
      target -> debuffs.curse_of_elements -> source = p;
    }
  }

  virtual bool ready()
  {
    if ( target -> debuffs.curse_of_elements -> check() )
      return false;

    return warlock_spell_t::ready();
  }
};

// Bane of Agony Spell ===========================================================

struct bane_of_agony_t : public warlock_spell_t
{
  bane_of_agony_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "bane_of_agony", player, "Bane of Agony" )
  {
    warlock_t* p = player -> cast_warlock();

    /*
     * BOA DATA:
     * No Glyph
     * 16 Ticks:
     * middle low low low low low    middle middle middle middle middle high high high high high
     * 15 ticks:
     * low    low low low low middle middle middle middle middle high   high high high high
     * 14 ticks:
     * low    low low low low middle middle middle middle high strange strange strange
     * 828 828 403 403 403 486 998 485 485 1167  691 1421 691 1316
     * 374 373 373 373 767 449 449 450 924  526  640  640 640 1316
     * 378 777 777 777 378 461 460 460 460  543 1370  666 666  666
     * 378 378 777 378 378 460 460 946 460  542 1369  667 666 1369
     *
     *
     * 13 ticks:
     * 2487 sp, lvl 80 -> middle = 130.14 + 0.1000000015 * 2487 = 378.84
     * -> difference = base_td / 2, only 1 middle tick at the beginning
     * 378 645 313 313 313 378 379 778 378 443 444 444 443
     * 12 ticks:
     * low low low low    middle middle middle middle   high high high high
     * and it looks like low-tick = middle-tick - base_td / 2
     * and              high-tick = middle-tick + base_td / 2
     * at 12 ticks, everything is very consistent with this logic
     */

    parse_options( NULL, options_str );

    base_crit += p -> talent_doom_and_gloom -> effect_base_value( 1 ) / 100.0;
    trigger_gcd -= p -> constants_pandemic_gcd * p -> talent_pandemic -> rank();

    int a_extra_ticks = (int) ( p -> glyphs.bane_of_agony -> base_value() / 1000.0 / base_tick_time );
    // after patch 3.0.8, the added ticks are double the base damage
    base_td = ( base_td * num_ticks + base_td * a_extra_ticks * 2 ) / (num_ticks + a_extra_ticks);
    num_ticks += a_extra_ticks;

  }

  virtual void modify_tick_damage()
  {
    warlock_spell_t::modify_tick_damage();

    // Modell ramp-up damage for number_ticks==12
    double difference = base_td /2;
    if (number_ticks==12 && current_tick <=4 )
      tick_dmg -= difference;
    if (number_ticks==12 && current_tick>=9)
      tick_dmg += difference;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute();
    if ( result_is_hit() )
      {
        if ( p -> dots_bane_of_doom -> ticking() )
          p -> dots_bane_of_doom -> action -> cancel();
        else if ( p -> buffs_bane_of_havoc -> up() )
        {
          p -> buffs_bane_of_havoc -> expire();
        }
      }
  }
};

// Bane of Doom Spell ===========================================================

struct bane_of_doom_t : public warlock_spell_t
{
  bane_of_doom_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "bane_of_doom", player, "Bane of Doom" )
  {
    parse_options( NULL, options_str );

    scale_with_haste = false;

    warlock_t* p = player -> cast_warlock();
    trigger_gcd -= p -> constants_pandemic_gcd * p -> talent_pandemic -> rank();
    base_crit += p -> talent_doom_and_gloom -> effect_base_value( 1 ) / 100.0;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute();

    if ( result_is_hit() )
    {
      if ( p -> dots_bane_of_agony -> ticking() )
        p -> dots_bane_of_agony -> action -> cancel();
      else if ( p -> buffs_bane_of_havoc -> up() )
      {
        p -> buffs_bane_of_havoc -> expire();
      }
    }
  }

  virtual void target_debuff( int dmg_type )
  {
    action_t::target_debuff( DMG_DIRECT );

    warlock_t* p = player -> cast_warlock();
    if ( p -> bugs && p -> buffs_demon_soul_felhunter -> up() )
    {
      target_multiplier *=  1.0 + p -> buffs_demon_soul_felhunter -> effect_base_value( 1 ) / 100.0;
    }
  }

  virtual double total_td_multiplier() SC_CONST
  {
    return warlock_spell_t::total_dd_multiplier();
  }

  virtual void tick()
  {
    warlock_spell_t::tick();

    warlock_t* p = player -> cast_warlock();
    double x = effect_base_value( 2 ) / 100.0 + p -> talent_impending_doom -> effect_base_value( 1 ) / 100.0;
    if ( p -> rng_ebon_imp -> roll ( x ) )
    {
      p -> procs_ebon_imp -> occur();
      p -> summon_pet( "ebon_imp", 15.0 );
    }
  }

  virtual void assess_damage( double amount,
                              int    dmg_type )
  {
    warlock_spell_t::assess_damage( amount, DMG_DIRECT );
  }
};

// Bane of Havoc Spell ===========================================================

struct bane_of_havoc_t : public warlock_spell_t
{
  bane_of_havoc_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "bane_of_havoc", player, "Bane of Havoc" )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    trigger_gcd -= p -> constants_pandemic_gcd * p -> talent_pandemic -> rank();
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute();
    if (result_is_hit())
    {
      p -> buffs_bane_of_havoc -> trigger();

      if ( p -> dots_bane_of_agony -> ticking() )
        p -> dots_bane_of_agony -> action -> cancel();
      else if ( p -> dots_bane_of_doom -> ticking() )
      p -> dots_bane_of_doom -> action -> cancel();
    }
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();
    if ( p -> buffs_bane_of_havoc -> up() )
      return false;

    return warlock_spell_t::ready();
  }

};

// Shadow Bolt Spell ===========================================================

struct shadow_bolt_t : public warlock_spell_t
{
  bool isb;

  shadow_bolt_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "shadow_bolt", player, "Shadow Bolt" ),
      isb( 0 )
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "isb",   OPT_BOOL, &isb   },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    base_execute_time += p -> talent_bane -> effect_base_value( 1 ) / 1000.0;
    base_cost  *= 1.0 + p -> glyphs.shadow_bolt -> base_value();
    base_multiplier *= 1.0 + ( p -> talent_shadow_and_flame -> effect_base_value( 2 ) / 100.0 );

    base_crit += p -> sets -> set ( SET_T10_2PC_CASTER ) -> effect_base_value( 1 ) / 100.0;
  }

  virtual double execute_time() SC_CONST
  {
    double h = warlock_spell_t::execute_time();
    warlock_t* p = player -> cast_warlock();
    if ( p -> buffs_shadow_trance -> up() ) h = 0;
    if ( p -> buffs_backdraft -> up() )
    {
      h *= 1.0 + p -> buffs_backdraft -> effect_base_value( 1 ) / 100.0;
    }
    return h;
  }

  virtual void schedule_execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::schedule_execute();
    p -> buffs_shadow_trance -> expire();
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute();

    for ( int i=0; i < 4; i++ )
    {
      p -> uptimes_backdraft[ i ] -> update( i == p -> buffs_backdraft -> stack() );
    }

    if ( p -> buffs_backdraft -> up() )
    {
      p -> buffs_backdraft -> decrement();
    }
    if ( p -> buffs_demon_soul_succubus -> up() )
    {
      target_multiplier *= 1.0 + p -> buffs_demon_soul_succubus -> effect_base_value( 1 ) / 100.0;
    }
  }

  virtual void travel( int travel_result, double travel_dmg)
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::travel(travel_result, travel_dmg);
    trigger_decimation( this, travel_result );
    trigger_impending_doom( this );
    if ( result_is_hit() )
    {
      p -> buffs_shadow_embrace -> trigger();
      target -> debuffs.improved_shadow_bolt -> trigger( 1, 1.0, p -> talent_shadow_and_flame -> effect_base_value( 1 ) / 100.0 );
      target -> debuffs.curse_of_elements -> source = p;
      trigger_everlasting_affliction( this );
    }
  }

  virtual bool ready()
  {
    if ( isb )
      if ( ! sim -> target -> debuffs.improved_shadow_bolt -> check() )
        return false;

    return warlock_spell_t::ready();
  }
};

// Burning Embers Spell ===========================================================

struct burning_embers_t : public warlock_spell_t
{

  burning_embers_t( player_t* player ) :
    warlock_spell_t( "burning_embers", player, 85421 )
  {
    tick_may_crit= false;
    scale_with_haste = false;
    reset();
  }

  virtual double calculate_tick_damage()
  {
    warlock_t* p = player -> cast_warlock();
    double cap;
    double a;

    cap = ( 0.5 * p -> talent_burning_embers -> rank() * p -> composite_spell_power( SCHOOL_MAX ) + p -> talent_burning_embers -> effect_misc_value1( 2 ) ) / 7.0;
    a = base_td * p -> talent_burning_embers -> effect_base_value( 1 ) / 100.0 + tick_dmg;

    if ( a > cap)
      tick_dmg = cap;
    else
      tick_dmg = a;

    return tick_dmg;
  }
};

// Trigger Burning Embers =========================================================
static void trigger_burning_embers ( spell_t* s, double dmg )
{
  warlock_t* p;
  if ( s -> player -> type == WARLOCK)
  {
    p = s -> player -> cast_warlock();
  }
  else
  {
    pet_t* a = ( pet_t* ) s -> player -> cast_pet();
    p = a -> owner -> cast_warlock();
  }

  if ( p -> talent_burning_embers -> rank() )
  {
    if ( p -> spells_burning_embers )
    {
      p -> spells_burning_embers -> base_td = dmg ;
      p -> spells_burning_embers -> execute();
    }
    else
    {
      p -> spells_burning_embers = new burning_embers_t( p );
      p -> spells_burning_embers -> base_td = dmg ;
      p -> spells_burning_embers -> execute();
    }

  }
}

// Chaos Bolt Spell ===========================================================

struct chaos_bolt_t : public warlock_spell_t
{
  chaos_bolt_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "chaos_bolt", player, "Chaos Bolt" )
  {
    parse_options( NULL, options_str );

    may_resist        = false;

    warlock_t* p = player -> cast_warlock();
    base_execute_time += p -> talent_bane -> effect_base_value( 1 ) / 1000.0;
    base_execute_time *= 1 + p -> sets -> set ( SET_T11_2PC_CASTER ) -> effect_base_value( 1 ) / 100.0;
    cooldown -> duration += ( p -> glyphs.chaos_bolt -> base_value() / 1000.0 );
  }

  virtual double execute_time() SC_CONST
  {
    double h = warlock_spell_t::execute_time();
    warlock_t* p = player -> cast_warlock();
    if ( p -> buffs_backdraft -> up() )
    {
      h *= 1.0 + p -> buffs_backdraft -> effect_base_value( 1 ) / 100.0;
    }
    return h;
  }

  virtual void execute()
  {
    warlock_spell_t::execute();
    warlock_t* p = player -> cast_warlock();

    for ( int i=0; i < 4; i++ )
    {
      p -> uptimes_backdraft[ i ] -> update( i == p -> buffs_backdraft -> stack() );
    }

    if ( p -> buffs_backdraft -> up() )
    {
      p -> buffs_backdraft -> decrement();
    }
    if ( result_is_hit() )
    {
      trigger_soul_leech( this );
    }
  }

  virtual void player_buff()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::player_buff();
    if ( p -> dots_immolate -> ticking() )
    {
      player_multiplier *= 1 + p -> talent_fire_and_brimstone -> effect_base_value( 1 ) / 100.0;
    }
  }
};

// Death Coil Spell ===========================================================

struct death_coil_t : public warlock_spell_t
{
  death_coil_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "death_coil", player, "Death Coil" )
  {
    parse_options( NULL, options_str );

    binary            = true;
  }

  virtual void execute()
  {
    warlock_spell_t::execute();
    if ( result_is_hit() )
    {
      player -> resource_gain( RESOURCE_HEALTH, direct_dmg );
    }
  }

};

// Shadow Burn Spell ===========================================================

struct shadowburn_t : public warlock_spell_t
{
  shadowburn_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "shadowburn", player, "Shadowburn" )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    check_talent( p -> talent_shadowburn -> rank() );
  }

  virtual void execute()
  {
    warlock_spell_t::execute();
    if ( result_is_hit() )
    {
      trigger_soul_leech( this );
    }
  }

  virtual void player_buff()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::player_buff();
    if ( p -> glyphs.shadowburn -> ok() )
    {
      if ( target -> health_percentage() < p -> glyphs.shadowburn -> effect_base_value( 1 ) )
      {
        cooldown -> reset();
      }
    }
  }

  virtual bool ready()
  {
    if ( target -> health_percentage() >= 20 )
      return false;

    return warlock_spell_t::ready();
  }
};

// Shadowfury Spell ===========================================================

struct shadowfury_t : public warlock_spell_t
{
  double cast_gcd;

  shadowfury_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "shadowfury", player, "Shadowfury" ),
    cast_gcd( -1 )
  {
    warlock_t* p = player -> cast_warlock();
    check_talent( p -> talent_shadowfury -> rank() );

    option_t options[] =
    {
      { "cast_gcd",    OPT_FLT,  &cast_gcd    },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    // estimate - measured at ~0.6sec, but lag in there too, plus you need to mouse-click
    trigger_gcd = ( cast_gcd >= 0 ) ? cast_gcd : 0.5;

  }
};


// Corruption Spell ===========================================================

struct corruption_t : public warlock_spell_t
{
 corruption_t( player_t* player, const std::string& options_str ) :
   warlock_spell_t( "corruption", player, "Corruption" )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    base_crit += p -> talent_everlasting_affliction -> effect_base_value( 2 ) / 100.0;
    base_multiplier *= 1.0 + ( p -> talent_improved_corruption -> effect_base_value( 1 ) ) / 100.0;
    base_crit += p -> sets -> set ( SET_T10_2PC_CASTER ) -> effect_base_value( 1 ) / 100.0;
  }

  virtual void tick()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::tick();
    p -> buffs_eradication -> trigger();
    if ( p -> buffs_shadow_trance -> trigger() )
      p -> procs_shadow_trance -> occur();
    if ( p -> talent_siphon_life -> rank() )
    {
      if ( p -> rng_siphon_life -> roll ( p -> talent_siphon_life -> proc_chance() ) )
      {
        p -> resource_gain( RESOURCE_HEALTH, p -> resource_max[ RESOURCE_HEALTH ] * 0.02 );
      }
    }
  }
};

// Drain Life Spell ===========================================================

struct drain_life_t : public warlock_spell_t
{
  int interrupt;
  drain_life_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "drain_life", player, "Drain Life" )
  {
    option_t options[] =
    {
      { "interrupt",  OPT_BOOL, &interrupt },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    channeled         = true;
    binary            = true;
    scale_with_haste  = false;
  }

  virtual void execute()
  {
    warlock_spell_t::execute();
    warlock_t* p = player -> cast_warlock();
    if ( result_is_hit() )
    {
      trigger_everlasting_affliction( this );
    }
    if ( p -> buffs_soulburn -> up() )
    {
      p -> buffs_soulburn -> expire();
    }
  }

  virtual double tick_time() SC_CONST
  {
    warlock_t* p = player -> cast_warlock();
    double t = warlock_spell_t::tick_time();

    if ( p -> buffs_soulburn -> check() )
    {
      t *= 1.0 - 0.5;
    }

    return t;
  }

  virtual void player_buff()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::player_buff();

    double min_multiplier[] = { 0, 0.03, 0.06 };
    double max_multiplier [3] = {0};
    if ( p -> bugs )
    {
      max_multiplier[0] =  0.00;
      max_multiplier[1] =  0.12;
      max_multiplier[2] =  0.24;
    }
    else
    {
      max_multiplier[0] =  0.00;
      max_multiplier[1] =  0.09;
      max_multiplier[2] =  0.18;
    }

    assert( p -> talent_soul_siphon -> rank() >= 0 &&
            p -> talent_soul_siphon -> rank() <= 2 );

    double min = min_multiplier[ p -> talent_soul_siphon -> rank() ];
    double max = max_multiplier[ p -> talent_soul_siphon -> rank() ];

    double multiplier = p -> affliction_effects() * min;

    if ( multiplier > max ) multiplier = max;

    player_multiplier *= 1.0 + multiplier;
  }

  virtual void tick()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::tick();
    if ( p -> buffs_shadow_trance -> trigger() )
      p -> procs_shadow_trance -> occur();

    if ( interrupt && ( current_tick != num_ticks ) )
    {
      // If any spell ahead of DS in the action list is "ready", then cancel the DS channel
      for ( action_t* action = p -> action_list; action != this; action = action -> next )
      {
        if ( action -> background ) continue;
        if ( action -> ready() )
        {
          current_tick = number_ticks;
          break;
        }
      }
    }
  }
};

// Drain Soul Spell ===========================================================

struct drain_soul_t : public warlock_spell_t
{
  int interrupt;

  drain_soul_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "drain_soul", player, "Drain Soul" )
  {
    option_t options[] =
    {
      { "interrupt",  OPT_BOOL, &interrupt },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    channeled         = true;
    binary            = true;
    scale_with_haste  = true; // informative
  }

  virtual void tick()
  {
    warlock_spell_t::tick();

    if ( result_is_hit() )
    {
      warlock_t* p = player -> cast_warlock();
      trigger_everlasting_affliction( this );
      if ( p -> talent_pandemic -> rank() )
      {
        if ( (target -> health_percentage() < effect_base_value( 3 ) ) && (p -> rng_pandemic -> roll( p -> talent_pandemic -> rank() * 0.5 ) ) )
        {
            if ( p -> dots_unstable_affliction -> ticking() )
            {
              p -> dots_unstable_affliction -> action -> refresh_duration();
            }
        }
      }
    }

    if ( interrupt && ( current_tick != num_ticks ) )
    {
      warlock_t* p = player -> cast_warlock();
      // If any spell ahead of DS in the action list is "ready", then cancel the DS channel
      for ( action_t* action = p -> action_list; action != this; action = action -> next )
      {
        if ( action -> background ) continue;
        if ( action -> ready() )
        {
          current_tick = number_ticks;
          break;
        }
      }
    }
  }

  virtual void player_buff()
  {
    warlock_spell_t::player_buff();

    warlock_t* p = player -> cast_warlock();

    double min_multiplier[] = { 0, 0.03, 0.06 };
    double max_multiplier [3] = {0};
    if ( p -> bugs )
    {
      max_multiplier[0] =  0.00;
      max_multiplier[1] =  0.12;
      max_multiplier[2] =  0.24;
    }
    else
    {
      max_multiplier[0] =  0.00;
      max_multiplier[1] =  0.09;
      max_multiplier[2] =  0.18;
    }

    assert( p -> talent_soul_siphon -> rank() >= 0 &&
            p -> talent_soul_siphon -> rank() <= 2 );

    double min = min_multiplier[ p -> talent_soul_siphon -> rank() ];
    double max = max_multiplier[ p -> talent_soul_siphon -> rank() ];

    double multiplier = p -> affliction_effects() * min;

    if ( multiplier > max ) multiplier = max;

    player_multiplier *= 1.0 + multiplier;

    // FIXME! Hack! Deaths Embrace is additive with Drain Soul "execute".
    // Perhaps it is time to add notion of "execute" into action_t class.

    double de_bonus = trigger_deaths_embrace( this );
    if ( de_bonus ) player_multiplier /= 1.0 + de_bonus;


      if ( target -> health_percentage() < effect_base_value( 3 ) )
      {
        player_multiplier *= 2.0 + de_bonus;
      }
  }
};

// Unstable Affliction Spell ======================================================

struct unstable_affliction_t : public warlock_spell_t
{
  unstable_affliction_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "unstable_affliction", player, "Unstable Affliction" )
  {
    parse_options( NULL, options_str );

    check_talent( ok() );

    warlock_t* p = player -> cast_warlock();
    base_crit += p -> talent_everlasting_affliction -> effect_base_value( 2 ) / 100.0;
    base_execute_time += p -> glyphs.unstable_affliction -> base_value() / 1000.0;
  }

  virtual void execute()
  {
    warlock_spell_t::execute();
    warlock_t* p = player -> cast_warlock();
    if ( result_is_hit() )
    {
      if ( p -> dots_immolate -> ticking() )
        p -> dots_immolate -> action -> cancel();
    }
  }

  virtual void tick()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::tick();
    if ( p -> set_bonus.tier10_4pc_caster() && tick_dmg > 0 )
    {
      p -> buffs_tier10_4pc_caster -> trigger();
    }
    if ( tick_dmg > 0 )
    {
      p -> buffs_tier11_4pc_caster -> trigger();
    }
  }

  virtual void extend_duration( int extra_ticks )
  {
    // Can't extend beyond initial duration. Assuming this limit is calculated based on current haste, not haste at previous application/extension/refresh.
    int remaining_ticks = number_ticks - ( current_tick - 1 ) + extra_ticks;
    int hasted_ticks = hasted_num_ticks();
    if ( remaining_ticks > hasted_ticks )
      extra_ticks -= ( remaining_ticks - hasted_ticks );
    if ( extra_ticks < 0 )
      extra_ticks = 0;
    warlock_spell_t::extend_duration( extra_ticks );
  }

};

// Haunt Spell ==============================================================

struct haunt_t : public warlock_spell_t
{
  haunt_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "Haunt", player, "Haunt" )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    check_talent( p -> talent_haunt -> rank() );

    base_execute_time *= 1 + p -> sets -> set ( SET_T11_2PC_CASTER ) -> effect_base_value( 1 ) / 100.0;
    direct_power_mod = 2 / 3.5;
  }

  virtual void travel( int travel_result, double travel_dmg)
  {
    warlock_spell_t::travel(travel_result, travel_dmg);
    if ( result_is_hit() )
    {
      warlock_t* p = player -> cast_warlock();
      p -> buffs_haunted -> trigger();
      p -> buffs_shadow_embrace -> trigger();
      trigger_everlasting_affliction( this );
    }
  }
};


// Immolate Spell =============================================================

struct immolate_t : public warlock_spell_t
{
  immolate_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "immolate", player, "Immolate", WARLOCK )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    base_execute_time += p -> talent_bane -> effect_base_value( 1 ) / 1000.0;

    base_dd_multiplier *= 1.0 + ( p -> talent_improved_immolate -> effect_base_value( 1 ) / 100.0 );
    base_td_multiplier *= 1.0 + ( p -> glyphs.immolate -> base_value() + p -> talent_improved_immolate -> effect_base_value( 1 ) / 100.0 );
  }

  virtual void execute()
  {
    warlock_spell_t::execute();
    if ( result_is_hit() )
    {
      warlock_t* p = player -> cast_warlock();
      if ( p -> dots_unstable_affliction -> ticking() )
        p -> dots_unstable_affliction -> action -> cancel();
    }
  }

  virtual void tick()
  {
    warlock_spell_t::tick();
    warlock_t* p = player -> cast_warlock();
    p -> buffs_molten_core -> trigger( 3 );
    if ( tick_dmg > 0 )
    {
      p -> buffs_tier10_4pc_caster -> trigger();
    }
    if ( tick_dmg > 0 )
    {
      p -> buffs_tier11_4pc_caster -> trigger();
    }
  }

  virtual void extend_duration( int extra_ticks )
  {
	// Can't extend beyond initial duration. Assuming this limit is calculated based on current haste, not haste at previous application/extension/refresh.
	int remaining_ticks = number_ticks - ( current_tick - 1 ) + extra_ticks;
	int hasted_ticks = hasted_num_ticks();
    if ( remaining_ticks > hasted_ticks ) extra_ticks -= ( remaining_ticks - hasted_ticks );
	if ( extra_ticks < 0 ) extra_ticks = 0;
	warlock_spell_t::extend_duration( extra_ticks );
  }

};

// Shadowflame DOT Spell =============================================================

struct shadowflame_dot_t : public warlock_spell_t
{
  shadowflame_dot_t( player_t* player ) :
    warlock_spell_t( "shadowflame_dot", player, 47960 )
  {
    dual       = true;
    proc       = true;
    background = true;
    stats = player -> get_stats( "shadowflame" );
  }
};
// Shadowflame Spell =============================================================

struct shadowflame_t : public warlock_spell_t
{
  shadowflame_dot_t* sf_dot;

  shadowflame_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "shadowflame", player, "Shadowflame" )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    sf_dot = new shadowflame_dot_t( p );
  }

  virtual void execute()
  {
    warlock_spell_t::execute();
    sf_dot -> execute();
  }
};

// Conflagrate Spell =========================================================

struct conflagrate_t : public warlock_spell_t
{
  conflagrate_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "conflagrate", player, "Conflagrate" )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    check_talent( p -> talent_conflagrate -> ok() );
    base_crit += p -> talent_fire_and_brimstone -> effect_base_value( 2 ) / 100.0;
    cooldown -> duration += ( p -> glyphs.conflagrate -> base_value() / 1000.0 );
    base_dd_multiplier *= 1.0 + ( p -> glyphs.immolate -> base_value() ) + ( p -> talent_improved_immolate -> effect_base_value( 1 ) / 100.0 );
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();

    double t = ( p -> dots_immolate -> action -> base_td + p -> dots_immolate -> action -> total_power() * p -> dots_immolate -> action -> tick_power_mod );

    base_dd_min  = t * ( p -> dots_immolate -> action -> number_ticks - p -> dots_immolate -> action -> added_ticks ) * effect_base_value( 2 ) / 100.0 ;
    base_dd_max  = base_dd_min;

    warlock_spell_t::execute();

    if ( result_is_hit() )
    {
      p -> buffs_backdraft -> trigger( 3 );
    }
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();

    if ( ! ( p -> dots_immolate -> ticking() ) ) return false;

    return warlock_spell_t::ready();
  }
};

// Incinerate Spell =========================================================

struct incinerate_t : public warlock_spell_t
{
  spell_t*  incinerate_burst_immolate;

  incinerate_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "incinerate", player, "Incinerate" )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    base_multiplier   *= 1.0 + ( p -> talent_shadow_and_flame -> effect_base_value( 2 ) / 100.0);
    base_execute_time += p -> talent_emberstorm -> effect_base_value( 3 ) / 1000.0;
    base_multiplier   *= 1.0 + ( p -> glyphs.incinerate -> base_value() );
    base_crit += p -> sets -> set ( SET_T10_2PC_CASTER ) -> effect_base_value( 1 ) / 100.0;
  }

  virtual void execute()
  {
    warlock_spell_t::execute();
    warlock_t* p = player -> cast_warlock();

    for ( int i=0; i < 4; i++ )
    {
      p -> uptimes_backdraft[ i ] -> update( i == p -> buffs_backdraft -> stack() );
    }

    if ( p -> buffs_backdraft -> up() )
    {
      p -> buffs_backdraft -> decrement();
    }
    trigger_impending_doom( this );
  }

  virtual void modify_direct_damage()
  {
    spell_t::modify_direct_damage();
    warlock_t* p = player -> cast_warlock();

    double divisor = 6.0;
    if ( p -> dots_immolate -> ticking() )
      direct_dmg += sim -> range( base_dd_min, base_dd_max ) / divisor * total_dd_multiplier();
  }

  virtual void travel( int travel_result, double travel_dmg)
  {
    warlock_spell_t::travel( travel_result, travel_dmg);
    trigger_decimation( this, travel_result );
  }

  virtual void player_buff()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::player_buff();

    if ( p -> buffs_molten_core -> up() ) {
      player_multiplier *= 1 + p -> buffs_molten_core -> effect_base_value( 1 ) / 100.0;
      p -> buffs_molten_core -> decrement();
    }

    if ( p -> dots_immolate -> ticking() )
    {
      player_multiplier *= 1 + p -> talent_fire_and_brimstone -> effect_base_value( 1 ) / 100.0;
    }
  }

  virtual double execute_time() SC_CONST
  {
    warlock_t* p = player -> cast_warlock();
    double h = warlock_spell_t::execute_time();

    if ( p -> buffs_molten_core -> up() )
    {
      h *= 1.0 + p -> buffs_molten_core -> effect_base_value( 3 ) / 100.0;
    }
    if ( p -> buffs_backdraft -> up() )
    {
      h *= 1.0 + p -> buffs_backdraft -> effect_base_value( 1 ) / 100.0;
    }

    return h;
  }
};

// Searing Pain Spell =========================================================

struct searing_pain_t : public warlock_spell_t
{
  searing_pain_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "searing_pain", player, "Searing Pain" )
  {
    parse_options( NULL, options_str );
  }

  virtual void player_buff()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::player_buff();

    if ( target -> health_percentage() <= 25 && p -> talent_improved_searing_pain -> rank() )
    {
      player_crit += p -> talent_improved_searing_pain -> effect_base_value( 1 ) / 100.0;
    }

    if ( p -> buffs_soulburn -> check() ) player_crit +=  p -> buffs_soulburn -> effect_base_value( 3 ) / 100.0;

    if ( p -> buffs_searing_pain_soulburn -> check() ) player_crit += p -> buffs_searing_pain_soulburn -> effect_base_value( 1 ) / 100.0;

  }

  virtual void execute()
  {
    warlock_spell_t::execute();
    warlock_t* p = player -> cast_warlock();

    if ( p -> buffs_soulburn -> up() )
    {
      p -> buffs_soulburn -> expire();
      p -> buffs_searing_pain_soulburn -> trigger();
    }
  }
};

// Soul Fire Spell ============================================================

struct soul_fire_t : public warlock_spell_t
{
  soul_fire_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "soul_fire", player, "Soul Fire" )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    base_execute_time += p -> talent_emberstorm -> effect_base_value( 1 ) / 1000.0;
    base_crit += p -> sets -> set ( SET_T10_2PC_CASTER ) -> effect_base_value( 1 ) / 100.0;
  }

  virtual void execute()
  {
    warlock_spell_t::execute();
    warlock_t* p = player -> cast_warlock();

    if ( p -> buffs_empowered_imp -> up() )
      p -> buffs_empowered_imp -> expire();
    else if ( p -> buffs_soulburn -> up() )
      p -> buffs_soulburn -> expire();
  }

  virtual double execute_time() SC_CONST
  {
    warlock_t* p = player -> cast_warlock();
    double t = warlock_spell_t::execute_time();

    if ( p -> buffs_decimation -> up() )
    {
      t *= 1.0 - p -> talent_decimation -> effect_base_value( 1 ) / 100.0;
    }
    if ( p -> buffs_empowered_imp -> up() )
    {
      t = 0;
    }
    if ( p -> buffs_soulburn -> check() )
    {
      t = 0;
    }

    return t;
  }

  virtual void travel( int travel_result, double travel_dmg)
  {
    warlock_spell_t::travel( travel_result, travel_dmg);

    trigger_decimation( this, travel_result );
    if ( result_is_hit() )
    {
      warlock_t* p = player -> cast_warlock();
      trigger_soul_leech( this );
      trigger_burning_embers( this, travel_dmg );

      if ( !p -> buffs.bloodlust -> up() )
        p -> buffs_improved_soul_fire -> trigger();
    }
  }
};

// Life Tap Spell ===========================================================

struct life_tap_t : public warlock_spell_t
{
  double trigger;
  double max_mana_pct;

  life_tap_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "life_tap", player, "Life Tap" ),
      trigger(0), max_mana_pct(0)
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "mana_percentage<", OPT_FLT,  &max_mana_pct     },
      { "trigger",          OPT_FLT,  &trigger          },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    harmful = false;

    if ( p -> glyphs.life_tap -> ok() )
      trigger_gcd += p -> glyphs.life_tap -> effect_base_value( 1 ) / 1000.0;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    if ( sim -> log ) log_t::output( sim, "%s performs %s", p -> name(), name() );

    double life = p -> resource_max[ RESOURCE_HEALTH ] * effect_base_value( 3 ) / 100.0;
    p -> resource_loss( RESOURCE_HEALTH, life );
    p -> resource_gain( RESOURCE_MANA, life * effect_base_value( 2 ) / 100.0, p -> gains_life_tap );
    if ( p -> talent_mana_feed -> rank() && p -> active_pet)
    {
      p -> active_pet -> resource_gain( RESOURCE_MANA, life * effect_base_value( 2 ) / 100.0 * p -> talent_mana_feed -> effect_base_value( 1 ) / 100.0, p -> active_pet -> gains_mana_feed );
    }
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();

    if (  max_mana_pct > 0 )
      if( ( 100.0 * p -> resource_current[ RESOURCE_MANA ] / p -> resource_max[ RESOURCE_MANA ] ) > max_mana_pct )
        return false;

    if ( trigger > 0 )
      if ( p -> resource_current[ RESOURCE_MANA ] > trigger )
        return false;

    return warlock_spell_t::ready();
  }
};

// Fel Armor Spell ==========================================================

struct fel_armor_t : public warlock_spell_t
{
  double bonus_spell_power;

  fel_armor_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "fel_armor", player, "Fel Armor" ), bonus_spell_power( 0 )
  {
    parse_options( NULL, options_str );

    harmful = false;

    warlock_t* p = player -> cast_warlock();
    bonus_spell_power = p -> buffs_fel_armor -> effect_min( 1 );

    // Model the passive health tick.....
    base_tick_time = effect_period( 2 );
    num_ticks      = 1;
    number_ticks   = 1;
    usable_pre_combat = true;
  }


  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();

    if ( sim -> log ) log_t::output( sim, "%s performs %s with %.1f spellpower", p -> name(), name(), bonus_spell_power );

    p -> buffs_fel_armor -> trigger( 1, bonus_spell_power );
    schedule_tick();
  }

  virtual void tick()
  {
    warlock_t* p = player -> cast_warlock();
    current_tick = 0; // ticks indefinitely
    p -> resource_gain( RESOURCE_HEALTH, p -> resource_max[ RESOURCE_HEALTH ] * effect_base_value( 2 ) / 100.0 * ( 1.0 + p -> talent_demonic_aegis -> effect_base_value( 1 ) / 100.0 ), p -> gains_fel_armor, this );
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();
    if ( p -> buffs_fel_armor -> up() )
      return false;

    return warlock_spell_t::ready();
  }
};

// Summon Pet Spell ==========================================================

struct summon_pet_t : public warlock_spell_t
{
  std::string pet_name;
  double summoning_duration;

  summon_pet_t( const char* n, player_t* player, const char* sname, const std::string& options_str ) :
    warlock_spell_t( n, player, sname ), pet_name(n), summoning_duration ( 0 )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    harmful = false;
    base_execute_time += p -> talent_master_summoner -> effect_base_value( 1 ) / 1000.0;
  }

  summon_pet_t( const char* n, player_t* player, int id ) :
    warlock_spell_t( n, player, id ), pet_name(n), summoning_duration ( 0 )
  {
    warlock_t* p = player -> cast_warlock();

    harmful = false;
    base_execute_time += p -> talent_master_summoner -> effect_base_value( 1 ) / 1000.0;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    p -> summon_pet( pet_name.c_str(), summoning_duration );
    warlock_spell_t::execute();
    if ( p -> buffs_soulburn -> up() )
    {
      p -> buffs_soulburn -> expire();
    }
  }

  virtual double execute_time() SC_CONST
  {
     warlock_t* p = player -> cast_warlock();
     double t = warlock_spell_t::execute_time();

     if ( p -> buffs_soulburn -> check() )
       t = 0.0;

     return t;
  }

};

// Summon Main Pet Spell ==========================================================

struct summon_main_pet_t : public summon_pet_t
{
  std::string pet_name;

  summon_main_pet_t( const char* n, player_t* player, const char* sname, const std::string& options_str ) :
    summon_pet_t( n, player, sname, options_str )
  {
    usable_pre_combat = true;
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();
    if ( p -> active_pet )
      return false;

    return summon_pet_t::ready();
  }
};

struct summon_felhunter_t : public summon_main_pet_t
{
  summon_felhunter_t( player_t* player, const std::string& options_str ) :
    summon_main_pet_t( "felhunter", player, "Summon Felhunter", options_str )
  { }
};

struct summon_felguard_t : public summon_main_pet_t
{
  summon_felguard_t( player_t* player, const std::string& options_str ) :
    summon_main_pet_t( "felguard", player, "Summon Felguard", options_str )
  {
    warlock_t* p = player -> cast_warlock();
    check_talent( p -> talent_summon_felguard -> ok() );
  }
};

struct summon_succubus_t : public summon_main_pet_t
{
  summon_succubus_t( player_t* player, const std::string& options_str ) :
    summon_main_pet_t( "succubus", player, "Summon Succubus", options_str )
  { }
};

struct summon_imp_t : public summon_main_pet_t
{
  summon_imp_t( player_t* player, const std::string& options_str ) :
    summon_main_pet_t( "imp", player, "Summon Imp", options_str )
  { }
};

struct summon_voidwalker_t : public summon_main_pet_t
{
  summon_voidwalker_t( player_t* player, const std::string& options_str ) :
    summon_main_pet_t( "voidwalker", player, "Summon Voidwalker", options_str )
  { }
};

// Infernal Awakening ============================================================

struct infernal_awakening_t : public warlock_spell_t
{
  infernal_awakening_t( player_t* player ) :
    warlock_spell_t( "Infernal_Awakening", player, 22703 )
  {
    aoe        = true;
    background = true;
    proc       = true;
    trigger_gcd= 0;
  }
};

// Summon Infernal Spell ==========================================================

struct summon_infernal_t : public summon_pet_t
{
  infernal_awakening_t* infernal_awakening;

  summon_infernal_t( player_t* player, const std::string& options_str  ) :
    summon_pet_t( "infernal", player, "Summon Infernal", options_str )
  {
    warlock_t* p = player -> cast_warlock();

    summoning_duration = duration() + p -> talent_ancient_grimoire -> effect_base_value( 1 ) / 1000.0;
    infernal_awakening = new infernal_awakening_t( p );
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    if ( infernal_awakening )
      infernal_awakening -> execute();
    p -> cooldowns_doomguard -> start();
    summon_pet_t::execute();
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();
    if ( !p -> buffs_metamorphosis -> check() && !p -> bugs)
      return false;

    return summon_pet_t::ready();
  }
};

// Summon Doomguard2 Spell =========================================================

struct summon_doomguard2_t : public summon_pet_t
{
  summon_doomguard2_t( player_t* player ) :
    summon_pet_t( "doomguard", player, 60478 )
  {
    warlock_t* p = player -> cast_warlock();
    harmful = false;
    background = true;
    summoning_duration = duration() + p -> talent_ancient_grimoire -> effect_base_value( 1 ) / 1000.0;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    p -> cooldowns_infernal -> start();
    summon_pet_t::execute();
  }
};

// Summon Doomguard Spell ==========================================================

struct summon_doomguard_t : public warlock_spell_t
{
  summon_doomguard2_t* summon_doomguard2;

  summon_doomguard_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "summon_doomguard", player, "Summon Doomguard" )
  {
    warlock_t* p = player -> cast_warlock();
    option_t options[] =
    {
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    harmful = false;
    summon_doomguard2 = new summon_doomguard2_t( p );
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    consume_resource();
    update_ready();
    p -> cooldowns_infernal -> start();
    summon_doomguard2 -> execute();
  }
};

// Immolation Damage Spell =====================================================

struct immolation_damage_t : public warlock_spell_t
{
  immolation_damage_t( player_t* player ) :
    warlock_spell_t( "immolation_dmg", player, 50590 )
  {
    dual        = true;
    background  = true;
    aoe         = true;
    direct_tick = true;
    may_crit    = false;

    stats = player -> get_stats( "immolation_aura" );
  }

  virtual void execute()
  {
    warlock_spell_t::execute();
    tick_dmg = direct_dmg;
    update_stats( DMG_OVER_TIME );
  }
};

// Immolation Aura Spell =======================================================

struct immolation_aura_t : public warlock_spell_t
{
  immolation_damage_t* immolation_damage;

  immolation_aura_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "immolation_aura", player, "Immolation Aura" )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    harmful = false;
    tick_may_crit = false;
    immolation_damage = new immolation_damage_t( p );
  }

  virtual void tick()
  {
    warlock_t* p = player -> cast_warlock();
    if ( p -> buffs_metamorphosis -> check() )
      immolation_damage -> execute();
    else
      current_tick = number_ticks;
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();
    if ( !p -> buffs_metamorphosis -> check() )
      return false;

    return warlock_spell_t::ready();
  }
};

// Metamorphosis Spell =======================================================

struct metamorphosis_t : public warlock_spell_t
{
  metamorphosis_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "metamorphosis", player, "Metamorphosis" )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    check_talent( p -> talent_metamorphosis -> rank() );
    trigger_gcd = 0;
    harmful = false;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    if ( sim -> log ) log_t::output( sim, "%s performs %s", p -> name(), name() );
    update_ready();
    p -> buffs_metamorphosis -> trigger();
  }
};

// Demonic Empowerment Spell ================================================

struct demonic_empowerment_t : public warlock_spell_t
{
  demonic_empowerment_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "demonic_empowerment", player, "Demonic Empowerment" )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    check_talent( p -> talent_demonic_empowerment -> rank() );
    harmful = false;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute();
    if( p -> active_pet -> pet_type == PET_FELGUARD )
    {
      p -> active_pet -> buffs.stunned -> expire();
    }
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();

    if ( ! p -> active_pet )
      return false;

    return warlock_spell_t::ready();
  }
};

// Hand of Gul'dan Spell =============================================================

struct hand_of_guldan_t : public warlock_spell_t
{

  hand_of_guldan_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "hand_of_guldan", player, "Hand of Gul'dan" )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    check_talent( p -> talent_hand_of_guldan -> rank() );
    base_execute_time *= 1 + p -> sets -> set ( SET_T11_2PC_CASTER ) -> effect_base_value( 1 ) / 100.0;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute();

    p -> buffs_hand_of_guldan -> trigger();
    trigger_impending_doom( this );

    if ( p -> dots_immolate -> ticking()  && p -> talent_cremation -> rank() )
    {
      if ( p -> rng_cremation -> roll( p -> talent_cremation -> proc_chance() ) )
      {
        p -> dots_immolate -> action -> refresh_duration();
      }
    }
  }
};

// Fel Flame Spell =========================================================

struct fel_flame_t : public warlock_spell_t
{

  fel_flame_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "fel_flame", player, "Fel Flame" )
  {
    parse_options( NULL, options_str );
  }

  virtual void execute()
  {
    warlock_spell_t::execute();
    warlock_t* p = player -> cast_warlock();
    if ( result_is_hit() )
    {
      if ( p -> dots_immolate -> ticking() )
        p -> dots_immolate -> action -> extend_duration( 2 );

      if ( p -> dots_unstable_affliction -> ticking() )
        p -> dots_unstable_affliction -> action -> extend_duration( 2 );
    }

  	if ( p -> buffs_tier11_4pc_caster -> check() )
  	{
  	  p -> buffs_tier11_4pc_caster -> expire();
  	}
  }

  virtual void player_buff()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::player_buff();

  	if ( p -> buffs_tier11_4pc_caster -> up() )
  	{
      player_crit += p -> buffs_tier11_4pc_caster -> effect_base_value( 1 ) / 100.0;
  	}
  }
};

// Dark Intent Spell =========================================================

struct dark_intent_t : public warlock_spell_t
{
  player_t*          dark_intent_target;

  dark_intent_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "dark_intent", player, "Dark Intent" ),
    dark_intent_target(0)
  {
    warlock_t* p = player -> cast_warlock();

    std::string target_str = p -> dark_intent_target_str;
    option_t options[] =
    {
      { "target", OPT_STRING, &target_str },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    usable_pre_combat = true;

    if ( target_str.empty() )
    {
      dark_intent_target = p;
    }
    else
    {
      dark_intent_target = sim -> find_player( target_str );

      assert ( dark_intent_target != 0 );
      assert ( dark_intent_target != p );
    }
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    if ( sim -> log ) log_t::output( sim, "%s performs %s", p -> name(), name() );
    if ( dark_intent_target == p )
    {
      if ( sim -> log ) log_t::output( sim, "%s grants SomebodySomewhere Dark Intent", p -> name() );
      p -> buffs.dark_intent_feedback -> override(3);
      if (p -> buffs.dark_intent -> check()) p -> buffs.dark_intent -> expire();
    p -> buffs.dark_intent -> override(1);
    }
    else
    {
      warlock_t* p = player -> cast_warlock();
      if ( sim -> log ) log_t::output( sim, "%s grants %s Dark Intent", p -> name(), dark_intent_target -> name() );
      dark_intent_target -> buffs.dark_intent -> trigger();
      dark_intent_target -> dark_intent_cb -> active = true;

      p -> buffs.dark_intent -> trigger();
      p -> dark_intent_cb -> active = true;
    }
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();

    if ( dark_intent_target == p )
    {
      if ( p -> buffs.dark_intent_feedback -> check() )
        return false;
    }
    else
    {
      if ( dark_intent_target -> buffs.dark_intent -> check() )
        return false;
    }

    return warlock_spell_t::ready();
  }
};

// Soulburn Spell =======================================================

struct soulburn_t : public warlock_spell_t
{
  soulburn_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "soulburn", player, "Soulburn" )
  {
    parse_options( NULL, options_str );

    harmful = false;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    if ( !( p -> use_pre_soulburn && p -> in_combat ) )
      p -> buffs_soulburn -> trigger();
    warlock_spell_t::execute();
  }
};

// Demon Soul Spell =======================================================

struct demon_soul_t : public warlock_spell_t
{
  demon_soul_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "demon_soul", player, "Demon Soul" )
  {
    parse_options( NULL, options_str );

    harmful = false;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    update_ready();
    assert ( p -> active_pet );

    if ( p -> active_pet -> pet_type == PET_IMP )
    {
      p -> buffs_demon_soul_imp -> trigger();
    }
    if ( p -> active_pet -> pet_type == PET_FELGUARD )
    {
      p -> buffs_demon_soul_felguard -> trigger();
    }
    if ( p -> active_pet -> pet_type == PET_FELHUNTER )
    {
      p -> buffs_demon_soul_felhunter -> trigger();
    }
    if ( p -> active_pet -> pet_type == PET_SUCCUBUS )
    {
      p -> buffs_demon_soul_succubus -> trigger();
    }
    if ( p -> active_pet -> pet_type == PET_VOIDWALKER )
    {
      p -> buffs_demon_soul_voidwalker -> trigger();
    }
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();
    if ( !p ->  active_pet)
      return false;

    return warlock_spell_t::ready();
  }
};

// Hellfire Effect Spell =====================================================

struct hellfire_tick_t : public warlock_spell_t
{
  hellfire_tick_t( player_t* player ) :
    warlock_spell_t( "hellfire_tick", player, 5857 )
  {
    dual        = true;
    background  = true;
    aoe         = true;
    direct_tick = true;

    warlock_t* p = player -> cast_warlock();
    base_multiplier *= 1.0 + p -> talent_cremation -> effect_base_value( 1 ) / 100.0;
    stats = player -> get_stats( "hellfire" );
  }

  virtual void execute()
  {
    warlock_spell_t::execute();
    tick_dmg = direct_dmg;
    update_stats( DMG_OVER_TIME );
  }
};

// Hellfire Spell ===========================================================

struct hellfire_t : public warlock_spell_t
{
  int interrupt;
  hellfire_tick_t* hellfire_tick;

  hellfire_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "hellfire", player, 1949 ), interrupt( 0 )
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "interrupt",  OPT_BOOL, &interrupt },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    // Hellfire has it's own damage effect, which is actually the damage to the player himself, so harmful is set to false.
    harmful = false;

    channeled         = true;
    scale_with_haste  = false;


    hellfire_tick = new hellfire_tick_t( p );

    if ( p -> talent_inferno -> rank() )
    {
      usable_moving   = true;
      range += p -> talent_inferno -> effect_base_value( 1 );
    }

  }

  virtual void tick()
  {
    hellfire_tick -> execute();

    if ( interrupt && ( current_tick != num_ticks ) )
    {
      warlock_t* p = player -> cast_warlock();
      // If any spell ahead of Hellfire in the action list is "ready", then cancel the Hellfire channel
      for ( action_t* action = p -> action_list; action != this; action = action -> next )
      {
        if ( action -> background ) continue;
        if ( action -> ready() )
        {
          current_tick = number_ticks;
          break;
        }
      }
    }
  }

};
// Seed of Corruption AOE Spell ======================================================

struct seed_of_corruption_aoe_t : public warlock_spell_t
{
  seed_of_corruption_aoe_t( player_t* player ) :
    warlock_spell_t( "seed_of_corruption", player, 27285 )

    {
      proc       = true;
      background = true;
      aoe        = true;

      name_str = "seed_of_corruption_aoe";
    }

    virtual void execute()
    {
      warlock_spell_t::execute();
      update_stats( DMG_DIRECT );
      warlock_t* p = player -> cast_warlock();
      if ( p -> buffs_soulburn -> up() && p -> talent_soulburn_seed_of_corruption -> rank() )
      {
        // Trigger Multiple Corruptions
        p -> buffs_soulburn -> expire();
      }
    }
};

// Seed of Corruption Spell ===========================================================

struct seed_of_corruption_t : public warlock_spell_t
{
  seed_of_corruption_aoe_t* seed_of_corruption_aoe;
  double dot_damage_done;

  seed_of_corruption_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "seed_of_corruption", player, "Seed of Corruption" )
  {
    parse_options( NULL, options_str );

    warlock_t* p = player -> cast_warlock();
    seed_of_corruption_aoe = new seed_of_corruption_aoe_t( p );

    base_crit += p -> talent_everlasting_affliction -> effect_base_value( 2 ) / 100.0;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute();

    dot_damage_done = target -> total_dmg;
    if ( p -> dots_corruption -> ticking() )
    {
      p -> dots_corruption -> action -> cancel();
    }
  }

  virtual void tick()
  {
    warlock_spell_t::tick();

    if ( target -> total_dmg - dot_damage_done > effect_base_value ( 2 ) )
    {
      dot_damage_done=0.0;
      seed_of_corruption_aoe -> execute();
      spell_t::cancel();
    }
  }

};

// Rain of Fire Tick Spell ======================================================

struct rain_of_fire_tick_t : public warlock_spell_t
{
  rain_of_fire_tick_t( player_t* player ) :
    warlock_spell_t( "rain_of_fire_tick", player, 42223 )

    {
    dual        = true;
    background  = true;
    aoe         = true;
    direct_tick = true;

    stats = player -> get_stats( "rain_of_fire" );
    }

    virtual void execute()
    {
      warlock_spell_t::execute();
      tick_dmg = direct_dmg;
      update_stats( DMG_OVER_TIME );
    }
};
// Rain of Fire Spell =======================================================

struct rain_of_fire_t : public warlock_spell_t
{
  rain_of_fire_tick_t* rain_of_fire_tick;

  rain_of_fire_t( player_t* player, const std::string& options_str ) :
    warlock_spell_t( "rain_of_fire", player, "Rain of Fire" )
  {
    parse_options( NULL, options_str );

    harmful=false;
    channeled         = true;
    warlock_t* p = player -> cast_warlock();

    rain_of_fire_tick = new rain_of_fire_tick_t( p );
  }

  virtual void tick()
   {
     warlock_spell_t::tick();

    rain_of_fire_tick -> execute();

  }
};

} // ANONYMOUS NAMESPACE ====================================================

// imp_pet_t::fire_bolt_t::execute ==========================================

void imp_pet_t::firebolt_t::travel( int travel_result, double travel_dmg)
{
  warlock_pet_spell_t::travel( travel_result, travel_dmg);
  warlock_t* o = player -> cast_pet() -> owner -> cast_warlock();

  if ( o -> buffs_empowered_imp -> trigger() ) o -> procs_empowered_imp -> occur();
  trigger_burning_embers ( this, travel_dmg );
  trigger_mana_feed ( this, travel_result );
}

// ==========================================================================
// Warlock Character Definition
// ==========================================================================

// warlock_t::composite_spell_power =========================================

double warlock_t::composite_spell_power( const school_type school ) SC_CONST
{
  double sp = player_t::composite_spell_power( school );

  sp += buffs_fel_armor -> value();

  return sp;
}

// warlock_t::composite_spell_haste ==========================================

double warlock_t::composite_spell_haste() SC_CONST
{
  double h = player_t::composite_spell_haste();

  if ( buffs_improved_soul_fire -> up())
  {
    if ( buffs.bloodlust -> up() )
      buffs_improved_soul_fire -> expire(); // hack to drop imp._soul_fire when bloodlust is triggered
    else
      h /=  1.0 + talent_improved_soul_fire -> effect_base_value( 1 ) / 100.0;
  }

  return h;
}

// warlock_t::matching_gear_multiplier =============================================

double warlock_t::matching_gear_multiplier( const attribute_type attr ) SC_CONST
{
  if ( ( attr == ATTR_INTELLECT ) && passive_spells.nethermancy -> ok() )
    return ( passive_spells.nethermancy -> effect_base_value( 1 ) / 100.0 );

  return 0.0;
}

// warlock_t::create_action =================================================

action_t* warlock_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
  if ( name == "chaos_bolt"          ) return new          chaos_bolt_t( this, options_str );
  if ( name == "conflagrate"         ) return new         conflagrate_t( this, options_str );
  if ( name == "corruption"          ) return new          corruption_t( this, options_str );
  if ( name == "bane_of_agony"       ) return new       bane_of_agony_t( this, options_str );
  if ( name == "bane_of_doom"        ) return new        bane_of_doom_t( this, options_str );
  if ( name == "curse_of_elements"   ) return new   curse_of_elements_t( this, options_str );
  if ( name == "death_coil"          ) return new          death_coil_t( this, options_str );
  if ( name == "demonic_empowerment" ) return new demonic_empowerment_t( this, options_str );
  if ( name == "drain_life"          ) return new          drain_life_t( this, options_str );
  if ( name == "drain_soul"          ) return new          drain_soul_t( this, options_str );
  if ( name == "fel_armor"           ) return new           fel_armor_t( this, options_str );
  if ( name == "haunt"               ) return new               haunt_t( this, options_str );
  if ( name == "immolate"            ) return new            immolate_t( this, options_str );
  if ( name == "immolation"          ) return new     immolation_aura_t( this, options_str );
  if ( name == "shadowflame"         ) return new         shadowflame_t( this, options_str );
  if ( name == "incinerate"          ) return new          incinerate_t( this, options_str );
  if ( name == "life_tap"            ) return new            life_tap_t( this, options_str );
  if ( name == "metamorphosis"       ) return new       metamorphosis_t( this, options_str );
  if ( name == "shadow_bolt"         ) return new         shadow_bolt_t( this, options_str );
  if ( name == "shadowburn"          ) return new          shadowburn_t( this, options_str );
  if ( name == "shadowfury"          ) return new          shadowfury_t( this, options_str );
  if ( name == "searing_pain"        ) return new        searing_pain_t( this, options_str );
  if ( name == "soul_fire"           ) return new           soul_fire_t( this, options_str );
  if ( name == "summon_felhunter"    ) return new    summon_felhunter_t( this, options_str );
  if ( name == "summon_felguard"     ) return new     summon_felguard_t( this, options_str );
  if ( name == "summon_succubus"     ) return new     summon_succubus_t( this, options_str );
  if ( name == "summon_voidwalker"   ) return new   summon_voidwalker_t( this, options_str );
  if ( name == "summon_imp"          ) return new          summon_imp_t( this, options_str );
  if ( name == "summon_infernal"     ) return new     summon_infernal_t( this, options_str );
  if ( name == "summon_doomguard"    ) return new    summon_doomguard_t( this, options_str );
  if ( name == "unstable_affliction" ) return new unstable_affliction_t( this, options_str );
  if ( name == "hand_of_guldan"      ) return new      hand_of_guldan_t( this, options_str );
  if ( name == "fel_flame"           ) return new           fel_flame_t( this, options_str );
  if ( name == "dark_intent"         ) return new         dark_intent_t( this, options_str );
  if ( name == "soulburn"            ) return new            soulburn_t( this, options_str );
  if ( name == "demon_soul"          ) return new          demon_soul_t( this, options_str );
  if ( name == "bane_of_havoc"       ) return new       bane_of_havoc_t( this, options_str );
  if ( name == "hellfire"            ) return new            hellfire_t( this, options_str );
  if ( name == "seed_of_corruption"  ) return new  seed_of_corruption_t( this, options_str );
  if ( name == "rain_of_fire"        ) return new        rain_of_fire_t( this, options_str );

  return player_t::create_action( name, options_str );
}

// warlock_t::create_pet =====================================================

pet_t* warlock_t::create_pet( const std::string& pet_name )
{
  pet_t* p = find_pet( pet_name );

  if ( p ) return p;

  if ( pet_name == "felguard"     ) return new    felguard_pet_t( sim, this );
  if ( pet_name == "felhunter"    ) return new   felhunter_pet_t( sim, this );
  if ( pet_name == "imp"          ) return new         imp_pet_t( sim, this );
  if ( pet_name == "succubus"     ) return new    succubus_pet_t( sim, this );
  if ( pet_name == "voidwalker"   ) return new  voidwalker_pet_t( sim, this );
  if ( pet_name == "infernal"     ) return new    infernal_pet_t( sim, this );
  if ( pet_name == "doomguard"    ) return new   doomguard_pet_t( sim, this );
  if ( pet_name == "ebon_imp"     ) return new    ebon_imp_pet_t( sim, this );

  return 0;
}

// warlock_t::create_pets ====================================================

void warlock_t::create_pets()
{
  create_pet( "felguard"  );
  create_pet( "felhunter" );
  create_pet( "imp"       );
  create_pet( "succubus"  );
  create_pet( "voidwalker");
  create_pet( "infernal"  );
  create_pet( "doomguard" );
  create_pet( "ebon_imp"  );
}

// warlock_t::init_talents =======================================================

void warlock_t::init_talents()
{

  // Affliction
  talent_unstable_affliction          = new passive_spell_t ( this, "unstable_affliction", "Unstable Affliction" );
  talent_doom_and_gloom               = new talent_t ( this, "doom_and_gloom", "Doom and Gloom" );
  talent_improved_life_tap            = new talent_t ( this, "improved_life_tap", "Improved Life Tap" );
  talent_improved_corruption          = new talent_t ( this, "improved_corruption", "Improved Corruption" );
  talent_jinx                         = new talent_t ( this, "jinx", "Jinx" );
  talent_soul_siphon                  = new talent_t ( this, "soul_siphon", "Soul Siphon" );
  talent_siphon_life                  = new talent_t ( this, "siphon_life", "Siphon Life" );
  talent_eradication                  = new talent_t ( this, "eradication", "Eradication" );
  talent_soul_swap                    = new talent_t ( this, "soul_swap", "Soul Swap" );
  talent_shadow_embrace               = new talent_t ( this, "shadow_embrace", "Shadow Embrace" );
  talent_deaths_embrace               = new talent_t ( this, "deaths_embrace", "Death's Embrace" );
  talent_nightfall                    = new talent_t ( this, "nightfall", "Nightfall" );
  talent_soulburn_seed_of_corruption  = new talent_t ( this, "soulburn_seed_of_corruption", "Soulburn: Seed of Corruption" );
  talent_everlasting_affliction       = new talent_t ( this, "everlasting_affliction", "Everlasting Affliction" );
  talent_pandemic                     = new talent_t ( this, "pandemic", "Pandemic" );
  talent_haunt                        = new talent_t ( this, "haunt", "Haunt" );

  // Demonology
  talent_summon_felguard      = new passive_spell_t ( this, "summon_felguard", "Summon Felguard" );
  talent_demonic_embrace      = new talent_t ( this, "demonic_embrace", "Demonic Embrace" );
  talent_dark_arts            = new talent_t ( this, "dark_arts", "Dark Arts" );
  talent_mana_feed            = new talent_t ( this, "mana_feed", "Mana Feed" );
  talent_demonic_aegis        = new talent_t ( this, "demonic_aegis", "Demonic Aegis" );
  talent_master_summoner      = new talent_t ( this, "master_summoner", "Master Summoner" );
  talent_impending_doom       = new talent_t ( this, "impending_doom", "Impending Doom" );
  talent_demonic_empowerment  = new talent_t ( this, "demonic_empowerment", "Demonic Empowerment" );
  talent_molten_core          = new talent_t ( this, "molten_core", "Molten Core" );
  talent_hand_of_guldan       = new talent_t ( this, "hand_of_guldan", "Hand of Gul'dan" );
  talent_aura_of_foreboding   = new talent_t ( this, "aura_of_foreboding", "Aura of Foreboding" );
  talent_ancient_grimoire     = new talent_t ( this, "ancient_grimoire", "Ancient Grimoire" );
  talent_inferno              = new talent_t ( this, "inferno", "Inferno" );
  talent_decimation           = new talent_t ( this, "decimation", "Decimation" );
  talent_cremation            = new talent_t ( this, "cremation", "Cremation" );
  talent_demonic_pact         = new talent_t ( this, "demonic_pact", "Demonic Pact" );
  talent_metamorphosis        = new talent_t ( this, "metamorphosis", "Metamorphosis" );

  // Destruction
  talent_conflagrate            = new passive_spell_t ( this, "conflagrate", "Conflagrate" );
  talent_bane                   = new talent_t ( this, "bane", "Bane" );
  talent_shadow_and_flame       = new talent_t ( this, "shadow_and_flame", "Shadow and Flame" );
  talent_improved_immolate      = new talent_t ( this, "improved_immolate", "Improved Immolate" );
  talent_improved_soul_fire     = new talent_t ( this, "improved_soul_fire", "Improved Soul Fire" );
  talent_emberstorm             = new talent_t ( this, "emberstorm", "Emberstorm" );
  talent_improved_searing_pain  = new talent_t ( this, "improved_searing_pain", "Improved Searing Pain" );
  talent_backdraft              = new talent_t ( this, "backdraft", "Backdraft" );
  talent_shadowburn             = new talent_t ( this, "shadowburn", "Shadowburn" );
  talent_burning_embers         = new talent_t ( this, "burning_embers", "Burning Embers" );
  talent_soul_leech             = new talent_t ( this, "soul_leech", "Soul Leech" );
  talent_fire_and_brimstone     = new talent_t ( this, "fire_and_brimstone", "Fire and Brimstone" );
  talent_shadowfury             = new talent_t ( this, "shadowfury", "Shadowfury" );
  talent_empowered_imp          = new talent_t ( this, "empowered_imp", "Empowered Imp" );
  talent_bane_of_havoc          = new talent_t ( this, "bane_of_havoc", "Bane of Havoc" );
  talent_chaos_bolt             = new talent_t ( this, "chaos_bolt", "Chaos Bolt" );

  player_t::init_talents();
}

// warlock_t::init_spells =======================================================

void warlock_t::init_spells()
{
  // New set bonus system
  uint32_t set_bonuses[N_TIER][N_TIER_BONUS] = {
    //  C2P    C4P    M2P    M4P    T2P    T4P
    { 70839, 70841,     0,     0,     0,     0 }, // Tier10
    { 89934, 89935,     0,     0,     0,     0 }, // Tier11
    {     0,     0,     0,     0,     0,     0 },
  };

  player_t::init_spells();

  // passive_spells =======================================================================================

  // Core
  passive_spells.shadow_mastery       = new passive_spell_t( this, "shadow_mastery", "Shadow Mastery" );
  passive_spells.demonic_knowledge    = new passive_spell_t( this, "demonic_knowledge", "Demonic Knowledge" );
  passive_spells.cataclysm            = new passive_spell_t( this, "cataclysm", "Cataclysm" );
  passive_spells.nethermancy          = new passive_spell_t( this, "nethermancy", 86091 );
  //Affliction
  passive_spells.doom_and_gloom       = new passive_spell_t(this, "doom_and_gloom", "Doom and Gloom", talent_doom_and_gloom );
  passive_spells.pandemic             = new passive_spell_t(this, "pandemic", "Pandemic", talent_pandemic );

  // Mastery
  mastery_spells.fiery_apocalypse     = new mastery_t( this, "fiery_apocalypse", "Fiery Apocalypse", TREE_DESTRUCTION );
  mastery_spells.potent_afflictions   = new mastery_t(this, "potent_afflictions", "Potent Afflictions", TREE_AFFLICTION );
  mastery_spells.master_demonologist  = new mastery_t( this, "master_demonologist", "Master Demonologist", TREE_DEMONOLOGY );

  constants_pandemic_gcd              = 0.25;

  // Prime
  glyphs.metamorphosis        = new glyph_t(this, "Glyph of Metamorphosis");
  glyphs.conflagrate          = new glyph_t(this, "Glyph of Conflagrate");
  glyphs.chaos_bolt           = new glyph_t(this, "Glyph of Chaos Bolt");
  glyphs.corruption           = new glyph_t(this, "Glyph of Corruption");
  glyphs.bane_of_agony        = new glyph_t(this, "Glyph of Bane of Agony");
  glyphs.felguard             = new glyph_t(this, "Glyph of Felguard");
  glyphs.haunt                = new glyph_t(this, "Glyph of Haunt");
  glyphs.immolate             = new glyph_t(this, "Glyph of Immolate");
  glyphs.imp                  = new glyph_t(this, "Glyph of Imp");
  glyphs.lash_of_pain         = new glyph_t(this, "Glyph of Lash of Pain");
  glyphs.incinerate           = new glyph_t(this, "Glyph of Incinerate");
  glyphs.shadowburn           = new glyph_t(this, "Glyph of Shadowburn");
  glyphs.unstable_affliction  = new glyph_t(this, "Glyph of Unstable Affliction");

  // Major
  glyphs.life_tap             = new glyph_t(this, "Glyph of Life Tap");
  glyphs.shadow_bolt          = new glyph_t(this, "Glyph of Shadow Bolt");

  sets                        = new set_bonus_array_t( this, set_bonuses );
}

// warlock_t::init_glyphs =====================================================

void warlock_t::init_glyphs()
{
  std::vector<std::string> glyph_names;
  int num_glyphs = util_t::string_split( glyph_names, glyphs_str, ",/" );

  for ( int i=0; i < num_glyphs; i++ )
  {
    std::string& n = glyph_names[ i ];

    if      ( n == "chaos_bolt"          ) glyphs.chaos_bolt -> enable();
    else if ( n == "conflagrate"         ) glyphs.conflagrate -> enable();
    else if ( n == "corruption"          ) glyphs.corruption -> enable();
    else if ( n == "bane_of_agony"       ) glyphs.bane_of_agony -> enable();
    else if ( n == "felguard"            ) glyphs.felguard -> enable();
    else if ( n == "felhunter"           );
    else if ( n == "haunt"               ) glyphs.haunt -> enable();
    else if ( n == "immolate"            ) glyphs.immolate -> enable();
    else if ( n == "imp"                 ) glyphs.imp -> enable();
    else if ( n == "incinerate"          ) glyphs.incinerate -> enable();
    else if ( n == "life_tap"            ) glyphs.life_tap -> enable();
    else if ( n == "metamorphosis"       ) glyphs.metamorphosis -> enable();
    else if ( n == "searing_pain"        );
    else if ( n == "shadow_bolt"         ) glyphs.shadow_bolt -> enable();
    else if ( n == "shadow_burn"         ) glyphs.shadowburn -> enable();
    else if ( n == "unstable_affliction" ) glyphs.unstable_affliction -> enable();
    else if ( n == "lash_of_pain"        ) glyphs.lash_of_pain -> enable();
    // minor glyphs, to prevent 'not-found' warning
    else if ( n == "curse_of_exhaustion" ) ;
    else if ( n == "curse_of_exhausion" )  ; // It's mis-spelt on the armory.
    else if ( n == "drain_soul" )          ;
    else if ( n == "enslave_demon" )       ;
    else if ( n == "healthstone" )         ;
    else if ( n == "howl_of_terror")       ;
    else if ( n == "kilrogg" )             ;
    else if ( n == "shadowflame" )         ;
    else if ( n == "soul_link" )           ;
    else if ( n == "souls" )               ;
    else if ( n == "soulstone" )           ;
    else if ( n == "unending_breath" )     ;
    else if ( n == "voidwalker" )          ;
    else if ( n == "ritual_of_souls" )     ;
    else if ( n == "demonic_circle" )      ;
    else if ( n == "soul_swap" )           ;
    else if ( n == "eye_of_kilrogg" )      ;
    else if ( n == "fear" )                ;
    else if ( ! sim -> parent )
    {
      sim -> errorf( "Player %s has unrecognized glyph %s\n", name(), n.c_str() );
    }
  }
}

// warlock_t::init_race ======================================================

void warlock_t::init_race()
{
  race = util_t::parse_race_type( race_str );
  switch ( race )
  {
  case RACE_HUMAN:
  case RACE_DWARF:
  case RACE_GNOME:
  case RACE_UNDEAD:
  case RACE_ORC:
  case RACE_BLOOD_ELF:
  case RACE_TROLL:
  case RACE_WORGEN:
  case RACE_GOBLIN:
    break;
  default:
    race = RACE_UNDEAD;
    race_str = util_t::race_type_string( race );
  }

  player_t::init_race();
}

// warlock_t::init_base ======================================================

void warlock_t::init_base()
{
  player_t::init_base();

  attribute_multiplier_initial[ ATTR_STAMINA ] *= 1.0 + talent_demonic_embrace -> effect_base_value ( 1 ) / 100.0;

  base_attack_power = -10;
  initial_attack_power_per_strength = 2.0;
  initial_spell_power_per_intellect = 1.0;

  health_per_stamina = 10;
  mana_per_intellect = 15;

  resource_base[ RESOURCE_SOUL_SHARDS ] = 3;

  diminished_kfactor    = 0.009830;
  diminished_dodge_capi = 0.006650;
  diminished_parry_capi = 0.006650;
}

// warlock_t::init_scaling ===================================================

void warlock_t::init_scaling()
{
  player_t::init_scaling();
  scales_with[ STAT_SPIRIT ] = 0;
  scales_with[ STAT_STAMINA ] = 1;
}

// warlock_t::init_buffs =====================================================

void warlock_t::init_buffs()
{
  player_t::init_buffs();

  buffs_backdraft             = new buff_t( this, talent_backdraft -> effect_trigger_spell( 1 ), "backdraft" );
  buffs_decimation            = new buff_t( this, talent_decimation -> effect_trigger_spell( 1 ), "decimation", talent_decimation -> ok() );
  buffs_demonic_empowerment   = new buff_t( this, "demonic_empowerment",   1 );
  buffs_empowered_imp         = new buff_t( this, 47283, "empowered_imp", talent_empowered_imp -> effect_base_value( 1 ) / 100.0 );
  buffs_eradication           = new buff_t( this, talent_eradication -> effect_trigger_spell( 1 ), "eradication", talent_eradication -> proc_chance() );
  buffs_haunted               = new buff_t( this, talent_haunt -> spell_id(), "haunted", talent_haunt -> rank() );
  buffs_metamorphosis         = new buff_t( this, 47241, "metamorphosis", talent_metamorphosis -> rank() );
  buffs_metamorphosis -> buff_duration += glyphs.metamorphosis -> base_value() / 1000.0;
  buffs_metamorphosis -> cooldown -> duration = 0;
  buffs_molten_core           = new buff_t( this, talent_molten_core -> effect_trigger_spell( 1 ), "molten_core", talent_molten_core -> rank() * 0.02 );
  buffs_shadow_embrace        = new buff_t( this, talent_shadow_embrace -> effect_trigger_spell( 1 ), "shadow_embrace", talent_shadow_embrace -> rank() );
  buffs_shadow_trance         = new buff_t( this, 17941, "shadow_trance", talent_nightfall -> proc_chance() +  glyphs.corruption -> base_value() / 100.0 );
  
  buffs_hand_of_guldan        = new buff_t( this, "hand_of_guldan",        1, 15.0, 0.0, talent_hand_of_guldan -> rank() );
  buffs_improved_soul_fire    = new buff_t( this, 85383, "improved_soul_fire", (talent_improved_soul_fire -> rank() > 0) );
  buffs_improved_soul_fire -> cooldown -> duration  = player_data.effect_base_value( player_data.spell_effect_id( 85113, 3 ) );
  buffs_soulburn              = new buff_t( this, 74434, "soulburn" );
  buffs_demon_soul_imp        = new buff_t( this, 79459, "demon_soul_imp" );
  buffs_demon_soul_felguard   = new buff_t( this, 79462, "demon_soul_felguard" );
  buffs_demon_soul_felhunter  = new buff_t( this, 79460, "demon_soul_felhunter" );
  buffs_demon_soul_succubus   = new buff_t( this, 79463, "demon_soul_succubus" );
  buffs_demon_soul_voidwalker = new buff_t( this, 79464, "demon_soul_voidwalker" );
  buffs_bane_of_havoc         = new buff_t( this, 80240, "bane_of_havoc" );
  buffs_searing_pain_soulburn = new buff_t( this, 79440, "searing_pain_soulburn" );
  buffs_fel_armor             = new buff_t( this, "fel_armor", "Fel Armor" );
  buffs_tier10_4pc_caster     = new buff_t( this, sets -> set ( SET_T10_4PC_CASTER ) -> effect_trigger_spell( 1 ), "tier10_4pc_caster", sets -> set ( SET_T10_4PC_CASTER ) -> proc_chance() );
  buffs_tier11_4pc_caster     = new buff_t( this, sets -> set ( SET_T11_4PC_CASTER ) -> effect_trigger_spell( 1 ), "tier11_4pc_caster", sets -> set ( SET_T11_4PC_CASTER ) -> proc_chance() );

}

// warlock_t::init_gains =====================================================

void warlock_t::init_gains()
{
  player_t::init_gains();

  gains_fel_armor  = get_gain( "fel_armor"  );
  gains_felhunter  = get_gain( "felhunter"  );
  gains_life_tap   = get_gain( "life_tap"   );
  gains_soul_leech = get_gain( "soul_leech" );
  gains_mana_feed  = get_gain( "mana_feed"  );
}

// warlock_t::init_uptimes ====================================================

void warlock_t::init_uptimes()
{
  player_t::init_uptimes();

  uptimes_backdraft[ 0 ]  = get_uptime( "backdraft_0" );
  uptimes_backdraft[ 1 ]  = get_uptime( "backdraft_1" );
  uptimes_backdraft[ 2 ]  = get_uptime( "backdraft_2" );
  uptimes_backdraft[ 3 ]  = get_uptime( "backdraft_3" );
}
// warlock_t::init_procs =====================================================

void warlock_t::init_procs()
{
  player_t::init_procs();
  procs_impending_doom   = get_proc( "impending_doom" );
  procs_empowered_imp    = get_proc( "empowered_imp"  );
  procs_shadow_trance    = get_proc( "shadow_trance"  );
  procs_ebon_imp         = get_proc( "ebon_imp"       );
}

// warlock_t::init_rng =======================================================

void warlock_t::init_rng()
{
  player_t::init_rng();

  rng_soul_leech              = get_rng( "soul_leech"             );
  rng_everlasting_affliction  = get_rng( "everlasting_affliction" );
  rng_pandemic                = get_rng( "pandemic"               );
  rng_cremation               = get_rng( "cremation"              );
  rng_impending_doom          = get_rng( "impending_doom"         );
  rng_siphon_life             = get_rng( "siphon_life"            );
  rng_ebon_imp                = get_rng( "ebon_imp_proc"          );
}

// warlock_t::init_actions ===================================================

void warlock_t::init_actions()
{
  if ( action_list_str.empty() )
  {
    // Flask
    // TO-DO: Revert to >= 80 once Cata is out
    if ( level > 80 )
      action_list_str += "/flask,type=draconic_mind";
    else if ( level >= 75 )
      action_list_str += "/flask,type=frost_wyrm";

    // Food
    // TO-DO: Revert to >= 80 once Cata is out
    if ( level > 80 ) action_list_str += "/food,type=seafood_magnifique_feast";
    else if ( level >= 70 ) action_list_str += "/food,type=fish_feast";

    // Armor
    if ( level >= 62 ) action_list_str += "/fel_armor";

    // Choose Pet
    if ( primary_tree() == TREE_DEMONOLOGY )
      action_list_str += "/summon_felguard";
    else if ( primary_tree() == TREE_DESTRUCTION )
      action_list_str += "/summon_imp";
    else if ( primary_tree() == TREE_AFFLICTION )
    {
      if ( glyphs.lash_of_pain -> ok() && level < 85 )
        action_list_str += "/summon_succubus";
      else if ( glyphs.imp -> ok() && level < 85 )
        action_list_str += "/summon_imp";
      else
        action_list_str += "/summon_felhunter";
    }
    else
      action_list_str += "/summon_imp";

    // Dark Intent
    if ( level >= 83 )
      action_list_str += "/dark_intent";

    // Snapshot Stats
    action_list_str += "/snapshot_stats";

    // Usable Item
    int num_items = ( int ) items.size();
    for ( int i=0; i < num_items; i++ )
    {
      if ( items[ i ].use.active() )
      {
        action_list_str += "/use_item,name=";
        action_list_str += items[ i ].name();
      }
    }

    // Race Skills
    if ( race == RACE_ORC )
    {
      action_list_str += "/blood_fury";
    }
    else if ( race == RACE_BLOOD_ELF )
    {
      action_list_str += "/arcane_torrent";
    }
    else if ( race == RACE_TROLL )
    {
      action_list_str += "/berserking";
    }

    // Choose Potion
    // TO-DO. Revert to >= 80 once Cata is out
    if ( level > 80 )
      action_list_str += "/volcanic_potion,if=buff.bloodlust.react|!in_combat";
    else if ( level >= 70 )
    {
      if ( primary_tree() == TREE_AFFLICTION )
        action_list_str += "/speed_potion,if=buff.bloodlust.react|!in_combat";
      else
        action_list_str += "/wild_magic_potion,if=buff.bloodlust.react|!in_combat";
    }

    // Demon Soul
    if ( level >= 85 )
      action_list_str += "/demon_soul";

    switch ( primary_tree() )
    {

    case TREE_AFFLICTION:

      if ( talent_haunt -> rank() ) action_list_str += "/haunt";
      action_list_str += "/soulburn,if=buff.bloodlust.down";
      if ( talent_improved_soul_fire -> ok() && level >= 54)
      {
        if ( talent_emberstorm -> ok() )
        {
          action_list_str += "/soul_fire,if=buff.improved_soul_fire.cooldown_remains<(cast_time+travel_time)&buff.bloodlust.down&!in_flight";
        } else
        {
          action_list_str += "/soul_fire,if=buff.soulburn.up";
        }
      }
      if ( level >= 12 ) action_list_str += "/bane_of_agony,if=target.time_to_die>=20&!ticking";
      action_list_str += "/corruption,if=!ticking|dot.corruption.remains<tick_time";
      action_list_str += "/unstable_affliction,if=(!ticking|dot.unstable_affliction.remains<(cast_time+tick_time))&target.time_to_die>=5";
      if ( level >= 50) action_list_str += "/summon_infernal";
      if ( talent_soul_siphon -> rank() ) action_list_str += "/drain_soul,interrupt=1,if=target.health_pct<=25";
      if ( level >= 85 ) action_list_str += "/drain_life,interrupt=1,if=buff.demon_soul_felhunter.up";
      action_list_str += "/life_tap,mana_percentage<=35";
      if ( talent_bane -> rank() == 3 )
      {
        action_list_str += "/shadow_bolt";
      } 
      else
      {
        action_list_str += "/shadow_bolt,if=buff.shadow_trance.react";
        action_list_str += "/drain_life,interrupt=1";
      }

    break;

    case TREE_DESTRUCTION:
      action_list_str += "/soulburn,if=buff.bloodlust.down";
      if ( talent_improved_soul_fire -> ok() && level >= 54)
      {
        action_list_str += "/soul_fire,if=buff.improved_soul_fire.cooldown_remains<(cast_time+travel_time)&buff.bloodlust.down&!in_flight";
      }
      if ( level >= 20 ) action_list_str += "/bane_of_doom,if=!ticking";
      action_list_str += "/immolate,time_to_die>=3,if=dot.immolate.remains<cast_time+gcd|!ticking";
      if ( talent_conflagrate -> ok() ) action_list_str += "/conflagrate";
      action_list_str += "/corruption,if=!ticking|dot.corruption.remains<gcd";
      if ( level >= 75) action_list_str += "/shadowflame";
      if ( talent_chaos_bolt -> ok() ) action_list_str += "/chaos_bolt";
      if ( level >= 54) action_list_str += "/soul_fire,if=buff.empowered_imp.react|buff.soulburn.up";
      if ( level >= 50) action_list_str += "/summon_infernal";
      if ( level >= 64) action_list_str += "/incinerate";else action_list_str += "/shadow_bolt";

    break;

    case TREE_DEMONOLOGY:
      if ( talent_hand_of_guldan -> ok() ) action_list_str += "/hand_of_guldan,if=dot.immolate.remains>0";
      action_list_str += "/soulburn,if=buff.metamorphosis.react";
      if ( level >= 54) {
        if ( talent_improved_soul_fire -> ok() )
        {
          action_list_str += "/soul_fire,if=buff.improved_soul_fire.cooldown_remains<(cast_time+travel_time)&buff.bloodlust.down&!in_flight";
        } else {
          action_list_str += "/soul_fire,if=buff.soulburn.react";
        }
      }
      if ( talent_metamorphosis -> ok() ) action_list_str += "/metamorphosis";
      action_list_str += "/immolate,time_to_die>=4,if=dot.immolate.remains<cast_time|!ticking";
      if ( level >= 20 ) action_list_str += "/bane_of_doom,time_to_die>=20,if=!ticking";
      if ( level >= 60) action_list_str += "/immolation,if=buff.metamorphosis.remains>10";
      action_list_str += "/corruption,if=!ticking|dot.corruption.remains<tick_time";
      if ( level >= 75) action_list_str += "/shadowflame";
      if ( talent_hand_of_guldan -> ok() ) action_list_str += "/hand_of_guldan";
      if ( level >= 64) action_list_str += "/incinerate,if=buff.molten_core.react";
      if ( level >= 54) action_list_str += "/soul_fire,if=buff.decimation.react";
      if ( level >= 50) action_list_str += "/summon_infernal";
      action_list_str += "/life_tap,mana_percentage<=35";
      action_list_str += "/shadow_bolt";

    break;

  default:
      action_list_str += "/bane_of_doom,time_to_die>=20,if=!ticking";
      action_list_str += "/corruption,if=!ticking|dot.corruption.remains<tick_time";
      action_list_str += "/immolate,if=!ticking|dot.immolate.remains<(cast_time+tick_time)";
      if ( level >= 50) action_list_str += "/summon_infernal";
      if ( level >= 64) action_list_str += "/incinerate";else action_list_str += "/shadow_bolt";
      if ( sim->debug ) log_t::output( sim, "Using generic action string for %s.", name() );
    break;
  }

    action_list_str += "/life_tap"; // to use when no mana or nothing else is possible

    action_list_default = 1;
  }

  player_t::init_actions();
}

void warlock_t::init_resources( bool force )
{
	player_t::init_resources( force );
	if ( active_pet ) active_pet -> init_resources( force );
}


// warlock_t::reset ==========================================================

void warlock_t::reset()
{
  player_t::reset();

  // Active
  active_pet                 = 0;
}

// warlock_t::create_expression =================================================

action_expr_t* warlock_t::create_expression( action_t* a, const std::string& name_str )
{
  return player_t::create_expression( a, name_str );
}

// warlock_t::get_talent_trees =============================================

std::vector<talent_translation_t>& warlock_t::get_talent_list()
{
    talent_list.clear();
    return talent_list;
}

// warlock_t::get_options ==================================================

std::vector<option_t>& warlock_t::get_options()
{
  if ( options.empty() )
  {
    player_t::get_options();

    option_t warlock_options[] =
    {
      // @option_doc loc=player/warlock/misc title="Misc"
      { "use_pre_soulburn",         OPT_BOOL,   &( use_pre_soulburn                    ) },
      { "dark_intent_target",       OPT_STRING, &( dark_intent_target_str              ) },
      { NULL, OPT_UNKNOWN, NULL }
    };

    option_t::copy( options, warlock_options );
  }
  return options;
}

// warlock_t::decode_set ===================================================

int warlock_t::decode_set( item_t& item )
{
  if ( item.slot != SLOT_HEAD      &&
       item.slot != SLOT_SHOULDERS &&
       item.slot != SLOT_CHEST     &&
       item.slot != SLOT_HANDS     &&
       item.slot != SLOT_LEGS      )
  {
    return SET_NONE;
  }

  const char* s = item.name();

  if ( strstr( s, "dark_coven"   ) ) return SET_T10_CASTER;
  if ( strstr( s, "shadowflame"  ) ) return SET_T11_CASTER;

  return SET_NONE;
}

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_warlock ================================================

player_t* player_t::create_warlock( sim_t* sim, const std::string& name, race_type r )
{
  return new warlock_t( sim, name, r );
}

// player_t::warlock_init ===================================================

void player_t::warlock_init( sim_t* sim )
{
  sim -> auras.demonic_pact         = new aura_t( sim, "Demonic Pact", 1 );
  sim -> auras.fel_intelligence     = new aura_t( sim, "Fel Intelligence", 1 );

  for ( target_t* t = sim -> target_list; t; t = t -> next )
  {
    t -> debuffs.improved_shadow_bolt = new     debuff_t( t, "Shadow Mastery", 1, 30.0 );
    t -> debuffs.curse_of_elements    = new coe_debuff_t( t );
  }
}

// player_t::warlock_combat_begin ===========================================

void player_t::warlock_combat_begin( sim_t* sim )
{
  if ( sim -> overrides.demonic_pact )
    sim -> auras.demonic_pact -> override();

  for ( target_t* t = sim -> target_list; t; t = t -> next )
  {
    if ( sim -> overrides.improved_shadow_bolt ) t -> debuffs.improved_shadow_bolt -> override();
    if ( sim -> overrides.curse_of_elements    ) t -> debuffs.curse_of_elements    -> override( 1, 8 );
  }

}

