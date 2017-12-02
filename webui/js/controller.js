// add the filter to your application module
//angular.module('myApp', ['filters']);
//
//angular.module('filters', [])
//	.filter('Filesize', function () {
//		return function (size) {
//			if (isNaN(size))
//				size = 0;
//
//			if (size < 1024)
//				return size + ' Bytes';
//
//			size /= 1024;
//
//			if (size < 1024)
//				return size.toFixed(2) + ' Kb';
//
//			size /= 1024;
//
//			if (size < 1024)
//				return size.toFixed(2) + ' Mb';
//
//			size /= 1024;
//
//			if (size < 1024)
//				return size.toFixed(2) + ' Gb';
//
//			size /= 1024;
//
//			return size.toFixed(2) + ' Tb';
//		};
//	});
//


var dtcraft = angular.module("dtcraft", ["ngRoute"]);

dtcraft.controller("ClusterController", function($scope,$http,$timeout,$interval){
  var timeoutPromise;
  $scope.$on("$destroy", function(){
    $timeout.cancel(timeoutPromise);
  }); 
  $scope.update = function() {
   $http.jsonp('/cluster?jsonp=JSON_CALLBACK')
     .success(function(data){
       $scope.master = data.master;
       $scope.agents = data.agents;
       //$scope.agents = {};
       //_.each(data.agents, function(agent){
       //  $scope.agents[agent.key] = agent;
       //});
     })  
     .error(function(data){
     })  
     .finally( function(){
       timeoutPromise = $timeout($scope.update, 1000);
     }); 
  }
  $scope.update();
});


dtcraft.config(['$routeProvider', function($routeProvider){
  $routeProvider
    .when('/cluster',
    {
      templateUrl: '/cluster.html'
    })
    .otherwise(
    {
    });
}]);

dtcraft.filter('ByteFormatter', function() {

  var BYTES_PER_KB = Math.pow(10, 3);
  var BYTES_PER_MB = Math.pow(10, 6);
  var BYTES_PER_GB = Math.pow(10, 9);
  var BYTES_PER_TB = Math.pow(10, 12);
  var BYTES_PER_PB = Math.pow(10, 15);
  // NOTE: Number.MAX_SAFE_INTEGER is 2^53 - 1

  return function(bytes) {
    if(bytes == null || isNaN(bytes)) {
      return '';
    } else if(bytes < BYTES_PER_KB) {
      return bytes.toFixed() + ' B';
    } else if(bytes < BYTES_PER_MB) {
      return (bytes / BYTES_PER_KB).toFixed(1) + ' KB';
    } else if(bytes < BYTES_PER_GB) {
      return (bytes / BYTES_PER_MB).toFixed(1) + ' MB';
    } else if(bytes < BYTES_PER_TB) {
      return (bytes / BYTES_PER_GB).toFixed(1) + ' GB';
    } else if(bytes < BYTES_PER_PB) {
      return (bytes / BYTES_PER_TB).toFixed(1) + ' TB';
    } else {
      return (bytes / BYTES_PER_PB).toFixed(1) + ' PB';
    }
  };
});


