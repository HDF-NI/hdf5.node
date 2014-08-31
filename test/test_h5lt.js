var assert = require("assert");
//var should = require("should");
var fs = require('fs');
var parseString = require('xml2js').parseString;
var util = require('util');

var hdf5 = require('hdf5');
var h5lt = require('h5lt');

var Access = {
  ACC_RDONLY :	0,	/*absence of rdwr => rd-only */
  ACC_RDWR :	1,	/*open for read and write    */
  ACC_TRUNC :	2,	/*overwrite existing files   */
  ACC_EXCL :	3,	/*fail if file already exists*/
  ACC_DEBUG :	4,	/*print debug info	     */
  ACC_CREAT :	5	/*create non-existing files  */
};

var CreationOrder = {
    H5P_CRT_ORDER_TRACKED : 1,
    H5P_CRT_ORDER_INDEXED : 2
};

var State = {
  COUNT : 0,
  TITLE : 1,
  DATA : 2
};

describe("testing lite interface ",function(){

    describe("create an h5, group and some datasets ",function(){
        // open hdf file
        var file;
        before(function(){
          file = new hdf5.File('./h5lt.h5', Access.ACC_TRUNC);
        });
        var group;
        it("should be -1 yet", function(){
            group=file.createGroup();
            group.id.should.equal(-1);
        });
        it("should be >0 ", function(){
            group.create('pmc', file);
            group.id.should.not.equal(-1);
        });
        it("should be Float64Array io ", function(){
            var buffer=new Float64Array(5);
            buffer[0]=1.0;
            buffer[1]=2.0;
            buffer[2]=3.0;
            buffer[3]=4.0;
            buffer[4]=5.0;
            h5lt.makeDataset(group.id, 'Refractive Index', buffer);
            var readBuffer=h5lt.readDataset(group.id, 'Refractive Index');
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(5);
            readBuffer.buffer.byteLength.should.match(buffer.buffer.byteLength);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });
        it("should be Float32Array io ", function(){
            var buffer=new Float32Array(5);
            buffer[0]=5.0;
            buffer[1]=4.0;
            buffer[2]=3.0;
            buffer[3]=2.0;
            buffer[4]=1.0;
            h5lt.makeDataset(group.id, 'Refractive Index f', buffer);
            var readBuffer=h5lt.readDataset(group.id, 'Refractive Index f');
            readBuffer.constructor.name.should.match('Float32Array');
            readBuffer.length.should.match(5);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });
        it("should be Int32Array io ", function(){
            var buffer=new Int32Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index l', buffer);
            var readBuffer=h5lt.readDataset(group.id, 'Refractive Index l');
            readBuffer.constructor.name.should.match('Int32Array');
            readBuffer.length.should.match(5);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });
        it("should be Uint32Array io ", function(){
            var buffer=new Uint32Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index ui', buffer);
            var readBuffer=h5lt.readDataset(group.id, 'Refractive Index ui');
            readBuffer.length.should.match(5);
            readBuffer.constructor.name.should.match('Uint32Array');
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });
        it("flush properties to h5 ", function(){
            group.getNumAttrs().should.equal(0);
            group[ 'Computed Heat of Formation' ]=100.0;
            group.flush();
            group.getNumAttrs().should.equal(1);
            group[ 'Computed Heat of Formation' ]=7.77;
            group.flush();
            group.getNumAttrs().should.equal(1);
            group.Status=256;
            group.flush();
            group.getNumAttrs().should.equal(2);
            group.Status=-1;
            group.flush();
            group.getNumAttrs().should.equal(2);
            group.Information="\"There are no solutions; there are only trade-offs.\" -- Thomas Sowell";
            group.flush();
            group.getNumAttrs().should.equal(3);
            console.dir(group.Information);
        });
        it("should close pmc ", function(){
            group.close();
        });
    });
    describe("create an h5, group and some datasets ",function(){
        before(function(){
          file = new hdf5.File('./roothaan.h5', Access.ACC_TRUNC);
        });
        it("open of Geometries should be >0", function(){
            var groupPMCServices=file.createGroup();
            groupPMCServices.create('pmcservices', file);
            var groupTargets=file.createGroup();
            groupTargets.create('pmcservices/sodium-icosanoate', file);
            groupTargets[ 'Computed Heat of Formation' ]=-221.78436098572274;
            groupTargets[ 'Computed Ionization Potential' ]=9.57689311885752;
            groupTargets[ 'Computed Total Energy' ]=-3573.674399276322;
            groupTargets.flush();
            var groupDocuments=file.createGroup();
            groupDocuments.create('pmcservices/sodium-icosanoate/Documents', file);
            var groupFrequencyData=file.createGroup();
            groupFrequencyData.create('pmcservices/sodium-icosanoate/Frequency Data', file);
            var groupTrajectories=file.createGroup();
            groupTrajectories.create('pmcservices/sodium-icosanoate/Trajectories', file);
            fs.readFile("./test/examples/sodium-icosanoate.xml", "ascii", function (err, data) {
            h5lt.makeDataset(groupDocuments.id, 'sodium-icosanoate.xml', data);
            });
            fs.readFile("./test/examples/sodium-icosanoate.xmol", "ascii", function (err, data) {
                var count=0;
                var numberOfDataLines;
                var title;
                var state=State.COUNT;
                var lineArr = data.trim().split("\n");
                var columnCount=0;
                var firstFrequency=true;
                var firstTrajectory=new Float64Array(3*numberOfDataLines);
                var lastTrajectory=new Float64Array(3*numberOfDataLines);
                var frequency=new Float64Array(3*numberOfDataLines);
                /* Loop over every line. */
                lineArr.forEach(function (line) {
                    switch(state)
                    {
                        case State.COUNT:
                            numberOfDataLines=parseInt(line);
                            firstTrajectory=new Float64Array(3*numberOfDataLines);
                            lastTrajectory=new Float64Array(3*numberOfDataLines);
                            frequency=new Float64Array(3*numberOfDataLines);
                            state=State.TITLE;
                            break;
                        case State.TITLE:
                            title=line;
                            state=State.DATA;
                            break;
                        case State.DATA:
                            var columnArr = line.split(" ");
                            columnArr.forEach(function (value) {
                            switch(columnCount)
                            {
                                case 0:
                                    break;
                                case 1:
                                case 2:
                                case 3:
                                    firstTrajectory[3*count+columnCount-1]=parseFloat(value);
                                    lastTrajectory[3*count+columnCount-1]=parseFloat(value);
                                    break;
                                case 4:
                                case 5:
                                case 6:
                                    frequency[3*count+columnCount-4]=parseFloat(value);
                                    if(columnCount===6)count++;
                                    break;
                            }
                            columnCount++;
                            if(columnCount===7)columnCount=0;
                            if(count === numberOfDataLines){
                            //console.dir(count);
                            //console.dir(title);
                            count=0;
                            if(firstFrequency)
                            {
                                var groupGeometries=file.createGroup();
                                groupGeometries.create('pmcservices/sodium-icosanoate/Trajectories/Geometries', file);
                                firstTrajectory.rank=2;
                                firstTrajectory.rows=numberOfDataLines;
                                firstTrajectory.columns=3;
                                h5lt.makeDataset(groupGeometries.id, '0', firstTrajectory);
                                lastTrajectory.rank=2;
                                lastTrajectory.rows=numberOfDataLines;
                                lastTrajectory.columns=3;
                                h5lt.makeDataset(groupGeometries.id, '1', lastTrajectory);
                                var groupFrequencies=file.createGroup();
                                groupFrequencies.create('pmcservices/sodium-icosanoate/Frequency Data/Frequencies', file);
                                firstFrequency=false;
                            }
                                var groupFrequencies=file.openGroup('pmcservices/sodium-icosanoate/Frequency Data/Frequencies');
                                groupFrequencies.open('pmcservices/sodium-icosanoate/Frequency Data/Frequencies', file);
                                frequency.rank=2;
                                frequency.rows=numberOfDataLines;
                                frequency.columns=3;
                                h5lt.makeDataset(groupFrequencies.id, title, frequency);
                                state=State.COUNT;
                            }
                            });
                            break;
                    }
                });
            });
        });
    });
    describe("create an xmol with frequency pulled from h5 ",function(){
        var file;
        before(function(){
          file = new hdf5.File('./roothaan.h5', Access.ACC_RDONLY);
        });
        var groupTarget;
        it("open of target should be >0", function(){
            groupTarget=file.openGroup('pmcservices/sodium-icosanoate', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.id.should.not.equal(-1);
        });
        it("getNumAttrs of groupTarget should be 3", function(){
//            console.dir(groupTarget);
            groupTarget.getNumAttrs().should.equal(3);
            groupTarget.refresh();
//            console.dir(groupTarget);
            
            it("readAttribute Computed Heat Of Formation should be -221.78436098572274", function(){
                groupTarget.refresh();
                groupTarget[ 'Computed Heat Of Formation' ].should.equal(-221.78436098572274);
                groupTarget[ 'Computed Ionization Potential' ].should.equal(9.57689311885752);
                groupTarget[ 'Computed Total Energy' ].should.equal(-3573.674399276322);
            });
        });
        it("open of Geometries should be >0", function(){
            var groupDocuments=file.openGroup('pmcservices/sodium-icosanoate/Documents');
            var xmlDocument=h5lt.readDataset(groupDocuments.id, 'sodium-icosanoate.xml');
            parseString(xmlDocument, function (err, result) {
            var molecule=result['cml:cml']['cml:molecule'][0];
            //console.dir(util.inspect(molecule['$']['title'], false, null));

            var elements=[];
            var elementIndex=0;
            for (var moleculeIndex = 0; moleculeIndex < molecule['cml:molecule'].length; moleculeIndex++)
            {
                var atoms=molecule['cml:molecule'][moleculeIndex]['cml:atomArray'][0]['cml:atom'];
                elements.length+=atoms.length;
                for (var index = 0; index < atoms.length; index++)
                {
                    elements[elementIndex]=util.inspect(atoms[index]['$']['elementType'], false, null);
                    elements[elementIndex]=elements[elementIndex].substr(1,elements[elementIndex].length -2);
                    elementIndex++;
                }
            }
            var groupGeometries=file.openGroup('pmcservices/sodium-icosanoate/Trajectories/Geometries');
            var array=groupGeometries.getMemberNamesByCreationOrder();
            var groupFrequencies=file.openGroup('pmcservices/sodium-icosanoate/Frequency Data/Frequencies');
            var frequencyNames=groupFrequencies.getMemberNamesByCreationOrder();
            
            array[1].should.equal("1");
            var xmolDocument="";
            var lastTrajectory=h5lt.readDataset(groupGeometries.id, array[1]);
            lastTrajectory.rank.should.equal(2);
            lastTrajectory.columns.should.equal(3);
                for (var frequencyIndex = 0; frequencyIndex < frequencyNames.length; frequencyIndex++)
                {
                    xmolDocument+=elements.length+'\n';
                    xmolDocument+=frequencyNames[frequencyIndex]+'\n';
                    var frequency=h5lt.readDataset(groupFrequencies.id, frequencyNames[frequencyIndex]);
                    for (var index = 0; index < elements.length; index++)
                    {
                        xmolDocument+=elements[index]+' '+lastTrajectory[3*index]+' '+lastTrajectory[3*index+1]+' '+lastTrajectory[3*index+2]+' '+frequency[3*index]+' '+frequency[3*index+1]+' '+frequency[3*index+2]+'\n';
                    }
                }
                xmolDocument.length.should.equal(1435803);
                fs.writeFile('sodium-icosanoate.xmol', xmolDocument, [flag='w'])
                fs.writeFile('sodium-icosanoate.xml', xmlDocument, [flag='w'])
                groupGeometries.close();
            });
        });
        var groupGeometries;
        it("open of Geometries should be >0", function(){
            groupGeometries=file.openGroup('pmcservices/sodium-icosanoate/Trajectories/Geometries');
            groupGeometries.id.should.not.equal(-1);
        });
        it("getNumAttrs of Geometries should be 0", function(){
            groupGeometries.getNumAttrs().should.equal(0);
        });
        it("getNumObjs of Geometries should be 2", function(){
            groupGeometries.getNumObjs().should.equal(2);
        });
        it("getMemberNames of Geometries should be 240 names in creation order", function(){
            var array=groupGeometries.getMemberNamesByCreationOrder();
            array[1].should.equal("1");
        });
        it("getNumAttrs of file should be 3", function(){
//            console.dir(file);
            file.getNumAttrs().should.equal(0);
            file.refresh();
//            console.dir(file);

        });
    });
    
});
