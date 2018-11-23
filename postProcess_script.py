#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jun 29 08:12:46 2017

@author: xavierb
"""
import post_processing_fx as pp


network_name = 'NBNetwork'
GM_name = 'FebNB'
community_name = 'NBCommunity'
recovery_name = 'damageTest'
fail_name = 'damageTest'
sim_root_folder = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/simulations/'

reco_folder = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/simulations/'
'''
n_day = pp.getMaxRecoveryTime(reco_folder)

pop = pp.getBuildingPop(pp.getCommunityGeometry(community_name))

sim_root_folder = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/' + 'GA' + '/'
build_pop = pp.getBuildingPop(pp.getCommunityGeometry(community_name))
recovery_result = pp.getBuildingRecoveryRawResults(sim_root_folder, build_pop, 0)
n_day = pp.getMaxHistoricalRecoveryTime(sim_root_folder)
'''
#result = pp.getBuildingRecoveryRawResults(reco_folder, pop, 0)

#stat = pp.getNumberPeopleRecoveryResults(result, n_day)

#building = pp.getNumberBuildingRecoveryResults(result, n_day)

#pp.plotNumberPeopleRecovery(building[0], building[2], n_day)

a = pp.plotSinglePredictiveScenario(GM_name, network_name,community_name, 3, False, True)

#pp.plotSingleHistoricalRecovery(recovery_name, network_name, community_name, fail_name, 'simulations')

#pp.plotSingleRecovery(GM_name, network_name, community_name)

