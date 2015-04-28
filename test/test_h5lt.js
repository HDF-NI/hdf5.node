var assert = require("assert");
//var should = require("should");
var fs = require('fs');
var parseString = require('xml2js').parseString;
var util = require('util');

var hdf5 = require('hdf5');
var h5lt = require('h5lt');

var Access = require('lib/globals.js').Access;
var CreationOrder = require('lib/globals.js').CreationOrder;
var State = require('lib/globals.js').State;
var H5OType = require('lib/globals.js').H5OType;
var HLType = require('lib/globals').HLType;
var H5Type = require('lib/globals.js').H5Type;


describe("testing lite interface ",function(){

    describe("create an h5, group and some datasets ",function(){
        // open hdf file
        var file;
        before(function(){
          file = new hdf5.File('./h5lt.h5', Access.ACC_TRUNC);
        });
        var group;
        it("should be >0 ", function(){
            group=file.createGroup('pmc');
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
            var readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Refractive Index');
//            console.log(readAsBuffer.readDoubleLE(8));
            readAsBuffer.readDoubleLE(4*8).should.equal(5.0);
        });
        it("should be node::Buffer io for double le", function(){
            try
            {
            var buffer=new Buffer(5*8, "binary");
            buffer.type=H5Type.H5T_NATIVE_DOUBLE;
            buffer.writeDoubleLE(1.0, 0);
            buffer.writeDoubleLE(2.0, 8);
            buffer.writeDoubleLE(3.0, 16);
            buffer.writeDoubleLE(4.0, 24);
            buffer.writeDoubleLE(5.0, 32);
            h5lt.makeDataset(group.id, 'Dielectric Constant', buffer);
            var readBuffer=h5lt.readDataset(group.id, 'Dielectric Constant');
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(5);
            buffer.rank=1;
            buffer.rows=5;
            
            var readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Dielectric Constant');
//            console.log(readAsBuffer.readDoubleLE(8));
            readAsBuffer.readDoubleLE(4*8).should.equal(5.0);
            }
            catch(err) {
                console.dir(err.message);
            }
        });
        it("should be node::Buffer io for double 2 rank data", function(){
            try
            {
            var buffer=new Buffer(6*8, "binary");
            buffer.type=H5Type.H5T_NATIVE_DOUBLE;
            buffer.writeDoubleLE(1.0, 0);
            buffer.writeDoubleLE(2.0, 8);
            buffer.writeDoubleLE(3.0, 16);
            buffer.writeDoubleLE(1.0, 24);
            buffer.writeDoubleLE(2.0, 32);
            buffer.writeDoubleLE(3.0, 40);
            buffer.rank=2;
            buffer.rows=3;
            buffer.columns=2;
            h5lt.makeDataset(group.id, 'Two Rank', buffer);
            var readBuffer=h5lt.readDataset(group.id, 'Two Rank');
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(6);
            
            var readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Two Rank');
//            console.log(readAsBuffer.readDoubleLE(8));
            readAsBuffer.readDoubleLE(4*8).should.equal(2.0);
            readBuffer.rows.should.match(3);
            readBuffer.columns.should.match(2);
            }
            catch(err) {
                console.dir(err.message);
            }
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
        it("should be Int16Array io ", function(){
            var buffer=new Int16Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index s', buffer);
            var readBuffer=h5lt.readDataset(group.id, 'Refractive Index s');
            readBuffer.constructor.name.should.match('Int16Array');
            readBuffer.length.should.match(5);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });
        it("should be Uint16Array io ", function(){
            var buffer=new Uint16Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index us', buffer);
            var readBuffer=h5lt.readDataset(group.id, 'Refractive Index us');
            readBuffer.length.should.match(5);
            readBuffer.constructor.name.should.match('Uint16Array');
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });
        it("should be Int8Array io ", function(){
            var buffer=new Int8Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index 8', buffer);
            var readBuffer=h5lt.readDataset(group.id, 'Refractive Index 8');
            readBuffer.constructor.name.should.match('Int8Array');
            readBuffer.length.should.match(5);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });
        it("should be Uint8Array io ", function(){
            var buffer=new Uint8Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index u8', buffer);
            var readBuffer=h5lt.readDataset(group.id, 'Refractive Index u8');
            readBuffer.length.should.match(5);
            readBuffer.constructor.name.should.match('Uint8Array');
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });
        it("flush properties to h5 ", function(done){
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
            done();
        });
        it("should close pmc ", function(done){
            group.close();
            done();
        });
    });
    describe("create an h5, group and some datasets ",function(){
        before(function(){
          file = new hdf5.File('./roothaan.h5', Access.ACC_TRUNC);
        });
        it("open of Geometries should be >0", function(){
            var groupPMCServices=file.createGroup('pmcservices');
            var groupTargets=file.createGroup('pmcservices/sodium-icosanoate');
            groupTargets[ 'Computed Heat of Formation' ]=-221.78436098572274;
            groupTargets[ 'Computed Ionization Potential' ]=9.57689311885752;
            groupTargets[ 'Computed Total Energy' ]=-3573.674399276322;
            groupTargets.flush();
            var groupDocuments=file.createGroup('pmcservices/sodium-icosanoate/Documents');
            var groupFrequencyData=file.createGroup('pmcservices/sodium-icosanoate/Frequency Data');
            var groupTrajectories=file.createGroup('pmcservices/sodium-icosanoate/Trajectories');
            fs.readFile("./test/examples/sodium-icosanoate.xml", "ascii", function (err, data) {
            h5lt.makeDataset(groupDocuments.id, 'sodium-icosanoate.xml', data);
            groupTrajectories.close();
            groupFrequencyData.close();
            groupDocuments.close();
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
                                var groupGeometries=file.createGroup('pmcservices/sodium-icosanoate/Trajectories/Geometries');
                                firstTrajectory.rank=2;
                                firstTrajectory.rows=numberOfDataLines;
                                firstTrajectory.columns=3;
                                firstTrajectory.Dipole=2.9;
                                h5lt.makeDataset(groupGeometries.id, '0', firstTrajectory);
                                lastTrajectory.rank=2;
                                lastTrajectory.rows=numberOfDataLines;
                                lastTrajectory.columns=3;
                                h5lt.makeDataset(groupGeometries.id, '1', lastTrajectory);
                                var groupFrequencies=file.createGroup('pmcservices/sodium-icosanoate/Frequency Data/Frequencies');
                                groupGeometries.close();
                                groupFrequencies.close();
                                firstFrequency=false;
                            }
                                var groupFrequencies=file.openGroup('pmcservices/sodium-icosanoate/Frequency Data/Frequencies');
//                                groupFrequencies.open('pmcservices/sodium-icosanoate/Frequency Data/Frequencies', file);
                                frequency.rank=2;
                                frequency.rows=numberOfDataLines;
                                frequency.columns=3;
                                h5lt.makeDataset(groupFrequencies.id, title, frequency);
                                state=State.COUNT;
                                groupFrequencies.close();
                            }
                            });
                            break;
                    }
                });
            });
            groupTargets.close();
            groupPMCServices.close();
        });
        it("Existing group should throw exception when trying to create again ", function(done){
            try
            {
                var groupTargets=file.createGroup('pmcservices/sodium-icosanoate');
                groupTargets.close();
            }
            catch(err) {
                err.message.should.equal("");
                console.dir(err.message);
            }
            try
            {
                var groupDocuments=file.createGroup('pmcservices/sodium-icosanoate/Documents');
                groupDocuments.close();
            }
            catch(err) {
                err.message.should.equal("");
                console.dir(err.message);
            }
            done();
        });
        after(function(){
          file.close();
        });
    });
    describe("create an xmol with frequency pulled from h5 ",function(){
        var file;
        before(function(){
          file = new hdf5.File('./roothaan.h5', Access.ACC_RDONLY);
        });
        var groupTarget;
        it("open of target should be >0", function(done){
            groupTarget=file.openGroup('pmcservices/sodium-icosanoate', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.id.should.not.equal(-1);
            console.dir(groupTarget.id);
            done();
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
        it("open of Geometries should be >0", function(done){
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
            groupGeometries.getDatasetType(array[1]).should.equal(HLType.HL_TYPE_LITE);
            var lastTrajectory=h5lt.readDataset(groupGeometries.id, array[1]);
            lastTrajectory.rank.should.equal(2);
            lastTrajectory.columns.should.equal(3);
                for (var frequencyIndex = 0; frequencyIndex < frequencyNames.length; frequencyIndex++)
                {
                    xmolDocument+=elements.length+'\n';
                    xmolDocument+=frequencyNames[frequencyIndex]+'\n';
                    groupFrequencies.getDatasetType(frequencyNames[frequencyIndex]).should.equal(HLType.HL_TYPE_LITE);
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
                groupFrequencies.close();
                groupTarget.close();
            });
            groupDocuments.close();
            done();
        });
        var groupGeometries;
        it("open of Geometries should be >0", function(done){
            groupGeometries=file.openGroup('pmcservices/sodium-icosanoate/Trajectories/Geometries');
            groupGeometries.id.should.not.equal(-1);
            done();
        });
        it("getNumAttrs of Geometries should be 0", function(done){
            groupGeometries.getNumAttrs().should.equal(0);
            done();
        });
        it("getNumObjs of Geometries should be 2", function(done){
            groupGeometries.getNumObjs().should.equal(2);
            done();
        });
        it("getMemberNames of Geometries should be 240 names in creation order", function(done){
            var array=groupGeometries.getMemberNamesByCreationOrder();
            array[1].should.equal("1");
            done();
        });
        it("Size of dataset '0' should be 186 ", function(done){
            groupGeometries.getDatasetType('0').should.equal(HLType.HL_TYPE_LITE);
            var readBuffer=h5lt.readDataset(groupGeometries.id, '0');
            'Float64Array'.should.match(readBuffer.constructor.name);
            var length=186;
            length.should.match(readBuffer.length);
            var value=2.9;
            value.should.match(readBuffer.Dipole);
            groupGeometries.close();
            done();
        });
        it("getNumAttrs of file should be 3", function(){
//            console.dir(file);
            file.getNumAttrs().should.equal(0);
            file.refresh();
//            console.dir(file);

        });
        after(function(){
          file.close();
        });
    });
    
});
