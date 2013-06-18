'use strict';

var EditorState = {
    CLEAN:0, // NO CHANGES
    DIRTY:1, // UNSAVED CHANGES
    SAVE:2, // SAVE IN PROGRESS
    LOAD:3, // LOADING
    READONLY:4
};

google.load('picker', '1');
gapi.load('drive-share');

angular.module('app', ['app.filters', 'app.services', 'app.directives'])
    .constant('saveInterval', 15000)
    .constant('appId', [['138383410967-nth7efn0m93bq48rndhb1b5d6sua0n1p.apps.googleusercontent.com']])
    .config(['$routeProvider', function ($routeProvider) {
    $routeProvider
        .when('/edit/:id', {templateUrl:'partials/editor.html', controller:EditorCtrl})
        .when('/create/:folderId', {templateUrl:'partials/editor.html', controller:EditorCtrl})
        .otherwise({redirectTo:'/edit/'});
}]);
