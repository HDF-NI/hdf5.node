

module.exports.load = function *(table) {
    return { 
        multiview: true
        ,
        body: "<div id=\"table-chart\"><script type=\"text/javascript\" src=\"js/d3.min.js\"></script><script type=\"text/javascript\" src=\"js/nv.d3.min.js\"></script><script type=\"text/javascript\" src=\"js/chart.js\"></script></div>"
    
    };
};
