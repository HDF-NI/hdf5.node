/*These lines are all chart setup.  Pick and choose which chart features you want to utilize. */
var chart;
nv.addGraph(function() {
    chart = nv.models.lineChart()
            .options({
                transitionDuration: 300,
                useInteractiveGuideline: true
            })
  ;

        var h5Table = document.getElementById('h5-table');
        var thList=h5Table.getElementsByTagName("th");
        var tbodyList=h5Table.getElementsByTagName("tbody");
        var records=tbodyList.item(0).getElementsByTagName("tr");
        var labelValues=[];
  var myData = [];//sinAndCos();   //You need data...
  for(var index=0;index<records.length;index++){
      var tdList=records.item(index).getElementsByTagName("td");
      
      labelValues.push(tdList.item(0).innerText);
      if(index===0){
          for(var tdIndex=1;tdIndex<tdList.length;tdIndex++){
          myData.push({});
          myData[tdIndex-1].values=[];
          myData[tdIndex-1].key=thList.item(tdIndex).innerText;
          //myData[tdIndex-1].color=
      }
          
      }
          for(var tdIndex=1;tdIndex<tdList.length;tdIndex++){
              myData[tdIndex-1].values.push({x: index, y: parseFloat(tdList.item(tdIndex).innerText)});
          }
      
  }
  chart.xAxis     //Chart x-axis settings
      .axisLabel(thList.item(0).innerText)
      .tickFormat(function(d){
            return labelValues[d];
        });

  chart.yAxis     //Chart y-axis settings
      .axisLabel('Intensity')
      .tickFormat(d3.format('.02f'));

  /* Done setting the chart up? Time to render it!*/
console.log("select #table-chart");
  d3.select('#table-chart').append('svg')    //Select the <svg> element you want to render the chart in.   
      .datum(myData)         //Populate the <svg> element with chart data...
      .call(chart);          //Finally, render the chart!

  //Update the chart when window resizes.
  nv.utils.windowResize(function() { chart.update() });
  return chart;
});

