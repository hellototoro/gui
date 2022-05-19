/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-19 16:05:49
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-05-19 16:32:33
 * @FilePath: /gui/application/cast_conf.h
 * @Description: None
 * @other: None
 */
#ifndef __CAST_CONF_H__
#define __CAST_CONF_H__

/**
 * Cast delta UI, config WIFI manual, please choose only one
 */
//#define SOLUTION_CAST_DELTA	 
/**
 * Cast fly UI, config WIFI automatically, please choose only one
 */
//#define SOLUTION_CAST_FLY

/**
 * Cast unify UI, include cast, media features, please choose only one
 */
#define SOLUTION_CAST_UNIFY

#if defined(SOLUTION_CAST_DELTA) && (defined(SOLUTION_CAST_FLY) || defined(SOLUTION_CAST_UNIFY))
#error "Choose SOLUTION_CAST_DELTA, can not choose other solution again!"
#endif
#if defined(SOLUTION_CAST_FLY) && (defined(SOLUTION_CAST_DELTA) || defined(SOLUTION_CAST_UNIFY))
#error "Choose SOLUTION_CAST_FLY, can not choose other solution again!"
#endif
#if defined(SOLUTION_CAST_UNIFY) && (defined(SOLUTION_CAST_DELTA) || defined(SOLUTION_CAST_FLY))
#error "Choose SOLUTION_CAST_UNIFY, can not choose other solution again!"
#endif


//#define NETWORK_SUPPORT

#define CAST_DLNA_ENABLE    0
//#define CAST_AIRPLAY_ENABLE
//#define CAST_MIRACAST_ENABLE





#endif