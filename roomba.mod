/*********************************************
 * OPL 12.8.0.0 Model
 * Author: aashima
 * Creation Date: Mar 14, 2018 at 3:25:20 PM
 *********************************************/
int numNodes = ...;
int numScenarios = ...;
int capacity = ...;
int rate = ...;

range Nodes = 0..numNodes+1;
range Tiles = 1..numNodes;
range Scenarios = 1..numScenarios;

float probScenarios[Scenarios] = ...;

dvar int dplus[Scenarios][Nodes];
dvar int dminus[Scenarios][Nodes];
dvar boolean x[Nodes][Nodes];

minimize
  sum(s in Scenarios, i in Nodes)
    probScenarios[s] * (dplus[s][i] + dminus[s][i]);
subject to {
	forall (i in Tiles)
	  ctEachTileVacuumedAtMostOnce:
	  sum(j in Nodes)
	    x[i][j] <= 1;
	ctLeavesChargingStationOnce:
	  sum(j in Nodes)
	    x[0][j] == 1;
	ctReturnsToChargingStation:
	  sum(i in Nodes)
	    x[i][numNodes+1] == 1;
	forall(k in Tiles)
	  ctLeavesNodeOnlyIfEntersNode:
	  sum(i in Nodes) x[i][k] == sum(j in Nodes) x[k][j];
	ctCannotPickupMoreDirtThanCapacity:
	sum(i in Tiles) rate * sum(j in Nodes)x[i][j] <= capacity;
	/*forall(i in Tiles, s in Scenarios)
	  ctBalanceDirtOnEachTileEachScenario:
	  dminus[s][i] - dplus[s][i] + rate * sum(j in Nodes)x[i][j] = */
	forall (s in Scenarios, i in Nodes)
	  ctNonNegativityDplus:
	  dplus[s][i] >= 0;
	forall (s in Scenarios, i in Nodes)
	  ctNonNegativityDminus:  
	  dminus[s][i] >= 0;
}    