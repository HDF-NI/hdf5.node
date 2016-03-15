'use strict';

require('co-mocha')(require('mocha'));
require("should");

const fs            = require('fs');
const parseString   = require('xml2js').parseString;
const util          = require('util');
const hdf5Lib       = require('..');
const globs         = require('../lib/globals');

const hdf5          = hdf5Lib.hdf5;
const h5lt          = hdf5Lib.h5lt;
const Access        = globs.Access;
const CreationOrder = globs.CreationOrder;
const State         = globs.State;
const HLType        = globs.HLType;
const H5Type        = globs.H5Type;

describe("testing lite interface ", function() {

    describe("create an h5, group and some datasets ", function() {
        // open hdf file
        let file;
        before(function*() {
          file = new hdf5.File('./h5lt.h5', Access.ACC_TRUNC);
        });
        let group;
        it("should be >0 ", function*() {
            group=file.createGroup('pmc');
            group.id.should.not.equal(-1);
        });
        it("should be Float64Array io ", function*() {
            const buffer=new Float64Array(5);
            buffer[0]=1.0;
            buffer[1]=2.0;
            buffer[2]=3.0;
            buffer[3]=4.0;
            buffer[4]=5.0;
            h5lt.makeDataset(group.id, 'Refractive Index', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index');
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(5);
            readBuffer.buffer.byteLength.should.match(buffer.buffer.byteLength);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Refractive Index');
            readAsBuffer.readDoubleLE(4*8).should.equal(5.0);
        });
        it("should be node::Buffer io for double le", function*() {
            const buffer=new Buffer(5*8, "binary");
            buffer.type=H5Type.H5T_NATIVE_DOUBLE;
            buffer.writeDoubleLE(1.0, 0);
            buffer.writeDoubleLE(2.0, 8);
            buffer.writeDoubleLE(3.0, 16);
            buffer.writeDoubleLE(4.0, 24);
            buffer.writeDoubleLE(5.0, 32);
            h5lt.makeDataset(group.id, 'Dielectric Constant', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Dielectric Constant');
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(5);
            buffer.rank=1;
            buffer.rows=5;

            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Dielectric Constant');
            readAsBuffer.readDoubleLE(4*8).should.equal(5.0);
        });
        it("should be node::Buffer io for double 2 rank data", function*() {
            const buffer=new Buffer(6*8, "binary");
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
            const readBuffer=h5lt.readDataset(group.id, 'Two Rank');
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(6);

            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Two Rank');
            readAsBuffer.readDoubleLE(4*8).should.equal(2.0);
            readBuffer.rows.should.match(3);
            readBuffer.columns.should.match(2);
        });
        it("should be Float32Array io ", function*() {
            const buffer=new Float32Array(5);
            buffer[0]=5.0;
            buffer[1]=4.0;
            buffer[2]=3.0;
            buffer[3]=2.0;
            buffer[4]=1.0;
            h5lt.makeDataset(group.id, 'Refractive Index f', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index f');
            readBuffer.constructor.name.should.match('Float32Array');
            readBuffer.length.should.match(5);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });
        it("should be Int32Array io ", function*() {
            const buffer=new Int32Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index l', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index l');
            readBuffer.constructor.name.should.match('Int32Array');
            readBuffer.length.should.match(5);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });
        it("should be Uint32Array io ", function*() {
            const buffer=new Uint32Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index ui', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index ui');
            readBuffer.length.should.match(5);
            readBuffer.constructor.name.should.match('Uint32Array');
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });
        it("should be Int16Array io ", function*() {
            const buffer=new Int16Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index s', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index s');
            readBuffer.constructor.name.should.match('Int16Array');
            readBuffer.length.should.match(5);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });
        it("should be Uint16Array io ", function*() {
            const buffer=new Uint16Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index us', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index us');
            readBuffer.length.should.match(5);
            readBuffer.constructor.name.should.match('Uint16Array');
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });
        it("should be Int8Array io ", function*() {
            const buffer=new Int8Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index 8', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index 8');
            readBuffer.constructor.name.should.match('Int8Array');
            readBuffer.length.should.match(5);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });
        it.skip("should be Uint8Array io ", function*() {
            const buffer=new Uint8Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index u8', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index u8');
            readBuffer.length.should.match(5);
            readBuffer.constructor.name.should.match('Uint8Array');
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
        });

        it("should be make a dataset with compression ", function*() {
            const buffer=new Float64Array(5);
            buffer[0]=1.0;
            buffer[1]=2.0;
            buffer[2]=3.0;
            buffer[3]=4.0;
            buffer[4]=5.0;
            h5lt.makeDataset(group.id, 'Compressed Index', buffer, { compression: 7});
            const readBuffer=h5lt.readDataset(group.id, 'Compressed Index');
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(5);
            readBuffer.buffer.byteLength.should.match(buffer.buffer.byteLength);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Refractive Index');
            readAsBuffer.readDoubleLE(4*8).should.equal(5.0);
        });

        it("should be make a dataset with cutom chunk size ", function*() {
            const buffer=new Float64Array(5);
            buffer[0]=1.0;
            buffer[1]=2.0;
            buffer[2]=3.0;
            buffer[3]=4.0;
            buffer[4]=5.0;
            h5lt.makeDataset(group.id, 'Custom-chunked Index', buffer, { chunkSize: 2});
            const readBuffer=h5lt.readDataset(group.id, 'Custom-chunked Index');
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(5);
            readBuffer.buffer.byteLength.should.match(buffer.buffer.byteLength);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Refractive Index');
            readAsBuffer.readDoubleLE(4*8).should.equal(5.0);
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
            done();
        });
        it("should close pmc ", function(done){
            group.close();
            done();
        });
        after(function*() {
          file.close();
        });
    });

    describe("create an h5, group and some datasets ", function() {
        let file;
        before(function*() {
          file = new hdf5.File('./roothaan.h5', Access.ACC_TRUNC);
        });

        it("open of Geometries should be >0", function*() {
            const groupPMCServices=file.createGroup('pmcservices');
            const groupTargets=file.createGroup('pmcservices/sodium-icosanoate');
            groupTargets[ 'Computed Heat of Formation' ]=-221.78436098572274;
            groupTargets[ 'Computed Ionization Potential' ]=9.57689311885752;
            groupTargets[ 'Computed Total Energy' ]=-3573.674399276322;
            groupTargets.flush();
            const groupDocuments      = file.createGroup('pmcservices/sodium-icosanoate/Documents');
            const groupFrequencyData  = file.createGroup('pmcservices/sodium-icosanoate/Frequency Data');
            const groupTrajectories   = file.createGroup('pmcservices/sodium-icosanoate/Trajectories');
            const sodiumIcosanoateXml = fs.readFileSync("./test/examples/sodium-icosanoate.xml", "ascii");
            h5lt.makeDataset(groupDocuments.id, 'sodium-icosanoate.xml', sodiumIcosanoateXml);
            groupTrajectories.close();
            groupFrequencyData.close();
            groupDocuments.close();

            const sodiumIcosanoateXmol = fs.readFileSync("./test/examples/sodium-icosanoate.xmol", "ascii");
            let count              = 0;
            let numberOfDataLines;
            let title;
            let state              = State.COUNT;
            const lineArr          =  sodiumIcosanoateXmol.trim().split("\n");
            let columnCount        = 0;
            let firstFrequency     = true;
            let firstTrajectory    = new Float64Array(3*numberOfDataLines);
            let lastTrajectory     = new Float64Array(3*numberOfDataLines);
            let frequency          = new Float64Array(3*numberOfDataLines);
            /* Loop over every line. */
            lineArr.forEach(function (line) {
                switch(state) {
                    case State.COUNT:
                        numberOfDataLines = parseInt(line);
                        firstTrajectory   = new Float64Array(3*numberOfDataLines);
                        lastTrajectory    = new Float64Array(3*numberOfDataLines);
                        frequency         = new Float64Array(3*numberOfDataLines);
                        state             = State.TITLE;
                        break;
                    case State.TITLE:
                        title=line;
                        state=State.DATA;
                        break;
                    case State.DATA:
                        const columnArr = line.split(" ");
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
                                if(columnCount===6){ count++; }
                                break;
                        }
                        columnCount++;
                        if(columnCount===7){columnCount=0;}
                        if(count === numberOfDataLines){
                        count=0;
                        if(firstFrequency)
                        {
                            try{
                            const groupGeometries=file.createGroup('pmcservices/sodium-icosanoate/Trajectories/Geometries');
                            firstTrajectory.rank=2;
                            firstTrajectory.rows=numberOfDataLines;
                            firstTrajectory.columns=3;
                            firstTrajectory.Dipole=2.9;
                            h5lt.makeDataset(groupGeometries.id, '0', firstTrajectory);
                            lastTrajectory.rank=2;
                            lastTrajectory.rows=numberOfDataLines;
                            lastTrajectory.columns=3;
                            h5lt.makeDataset(groupGeometries.id, '1', lastTrajectory);
                            const groupFrequencies=file.createGroup('pmcservices/sodium-icosanoate/Frequency Data/Frequencies');
                            groupGeometries.close();
                            groupFrequencies.close();
                            firstFrequency=false;
                            }
                            catch(err){
                                console.dir("what? "+err.message);
                            }
                        }
                            const groupFrequencies=file.openGroup('pmcservices/sodium-icosanoate/Frequency Data/Frequencies');
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
            groupTargets.close();
            groupPMCServices.close();
        });

        it("Existing group should throw exception when trying to create again ", function(done){
            try {
                const groupTargets=file.createGroup('pmcservices/sodium-icosanoate');
                groupTargets.close();
            } catch(err) {
                err.message.should.equal("");
                console.dir(err.message);
            }
            try {
                const groupDocuments=file.createGroup('pmcservices/sodium-icosanoate/Documents');
                groupDocuments.close();
            } catch(err) {
                err.message.should.equal("");
                console.dir(err.message);
            }
            done();
        });

        after(function*() {
          file.close();
        });
    });

    describe("create an xmol with frequency pulled from h5 ", function() {
        let file;
        before(function*() {
          file = new hdf5.File('./roothaan.h5', Access.ACC_RDONLY);
        });
        let groupTarget;
        it("open of target should be >0", function*() {
            groupTarget=file.openGroup('pmcservices/sodium-icosanoate', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.id.should.not.equal(-1);
        });
        it("getNumAttrs of groupTarget should be 3", function*() {
            groupTarget.getNumAttrs().should.equal(3);
            groupTarget.refresh();

            it("readAttribute Computed Heat Of Formation should be -221.78436098572274", function*() {
                groupTarget.refresh();
                groupTarget[ 'Computed Heat Of Formation' ].should.equal(-221.78436098572274);
                groupTarget[ 'Computed Ionization Potential' ].should.equal(9.57689311885752);
                groupTarget[ 'Computed Total Energy' ].should.equal(-3573.674399276322);
            });
        });
        it.skip("open of Geometries should be >0", function*(){
            const groupDocuments = file.openGroup('pmcservices/sodium-icosanoate/Documents');
            const xmlDocument    = h5lt.readDataset(groupDocuments.id, 'sodium-icosanoate.xml');
            parseString(xmlDocument, function (err, result) {
            const molecule=result['cml:cml']['cml:molecule'][0];

            const elements     =[];
            let elementIndex =0;
            for (let moleculeIndex = 0; moleculeIndex < molecule['cml:molecule'].length; moleculeIndex++)
            {
                const atoms=molecule['cml:molecule'][moleculeIndex]['cml:atomArray'][0]['cml:atom'];
                elements.length+=atoms.length;
                for (let index = 0; index < atoms.length; index++)
                {
                    elements[elementIndex] = util.inspect(atoms[index].$.elementType, false, null);
                    elements[elementIndex] = elements[elementIndex].substr(1,elements[elementIndex].length -2);
                    elementIndex++;
                }
            }
            const groupGeometries=file.openGroup('pmcservices/sodium-icosanoate/Trajectories/Geometries');
            const array=groupGeometries.getMemberNamesByCreationOrder();
            const groupFrequencies=file.openGroup('pmcservices/sodium-icosanoate/Frequency Data/Frequencies');
            const frequencyNames=groupFrequencies.getMemberNamesByCreationOrder();

            array[1].should.equal("1");
            let xmolDocument="";
            groupGeometries.getDatasetType(array[1]).should.equal(HLType.HL_TYPE_LITE);
            const lastTrajectory=h5lt.readDataset(groupGeometries.id, array[1]);
            lastTrajectory.rank.should.equal(2);
            lastTrajectory.columns.should.equal(3);
                for (let frequencyIndex = 0; frequencyIndex < frequencyNames.length; frequencyIndex++)
                {
                    xmolDocument+=elements.length+'\n';
                    xmolDocument+=frequencyNames[frequencyIndex]+'\n';
                    groupFrequencies.getDatasetType(frequencyNames[frequencyIndex]).should.equal(HLType.HL_TYPE_LITE);
                    const frequency=h5lt.readDataset(groupFrequencies.id, frequencyNames[frequencyIndex]);
                    for (let index = 0; index < elements.length; index++)
                    {
                        xmolDocument+=elements[index]+' '+lastTrajectory[3*index]+' '+lastTrajectory[3*index+1]+' '+lastTrajectory[3*index+2]+' '+frequency[3*index]+' '+frequency[3*index+1]+' '+frequency[3*index+2]+'\n';
                    }
                }
                xmolDocument.length.should.equal(1435803);
                fs.writeFileSync('sodium-icosanoate.xmol', xmolDocument, { flag:'w'});
                fs.writeFileSync('sodium-icosanoate.xml', xmlDocument, { flag:'w'});
                groupGeometries.close();
                groupFrequencies.close();
                groupTarget.close();
            });
            groupDocuments.close();
        });

        let groupGeometries;
        it("open of Geometries should be >0", function*(){
            groupGeometries=file.openGroup('pmcservices/sodium-icosanoate/Trajectories/Geometries');
            groupGeometries.id.should.not.equal(-1);
        });
        it("getNumAttrs of Geometries should be 0", function*(){
            groupGeometries.getNumAttrs().should.equal(0);
        });
        it("getNumObjs of Geometries should be 2", function*(){
            groupGeometries.getNumObjs().should.equal(2);
        });
        it("getMemberNames of Geometries should be 240 names in creation order", function*(){
            const array=groupGeometries.getMemberNamesByCreationOrder();
            array[1].should.equal("1");
        });
        it("Size of dataset '0' should be 186 ", function*() {
            groupGeometries.getDatasetType('0').should.equal(HLType.HL_TYPE_LITE);
            const readBuffer=h5lt.readDataset(groupGeometries.id, '0');
            'Float64Array'.should.match(readBuffer.constructor.name);
            const length=186;
            length.should.match(readBuffer.length);
            const value=2.9;
            value.should.match(readBuffer.Dipole);
            groupGeometries.close();
        });
        it("getNumAttrs of file should be 3", function*() {
            file.getNumAttrs().should.equal(0);
            file.refresh();
        });
        after(function*() {
          file.close();
        });
    });

    describe.skip("create dataset and extract subset", function() {
        let file;
        before(function*() {
          file = new hdf5.File('./roothaan.h5', Access.ACC_RDWR);
        });

        it("should be node::Buffer io for double 2 rank data", function*() {
            const group=file.openGroup('pmcservices');
            const buffer=new Buffer(8*10*8, "binary");
            buffer.rank=2;
            buffer.rows=8;
            buffer.columns=10;
            buffer.type=H5Type.H5T_NATIVE_DOUBLE;
            for (let j = 0; j < buffer.columns; j++) {
                for (let i = 0; i < buffer.rows; i++){
                        if (j< (buffer.columns/2)) {
                            buffer.writeDoubleLE(1.0, 8*(i*buffer.columns+j));
                        } else {
                            buffer.writeDoubleLE(2.0, 8*(i*buffer.columns+j));
                        }
                }
            }

            h5lt.makeDataset(group.id, 'Waldo', buffer);
            const subsetBuffer=new Buffer(3*4*8, "binary");
            subsetBuffer.rank=2;
            subsetBuffer.rows=3;
            subsetBuffer.columns=4;
            subsetBuffer.type=H5Type.H5T_NATIVE_DOUBLE;
            for (let j = 0; j < subsetBuffer.columns; j++) {
                for (let i = 0; i < subsetBuffer.rows; i++){
                            subsetBuffer.writeDoubleLE(5.0, 8*(i*subsetBuffer.columns+j));
                }
            }

            h5lt.writeDataset(group.id, 'Waldo', subsetBuffer, {start: [1,2], stride: [1,1], count: [3,4]});
            const readBuffer=h5lt.readDataset(group.id, 'Waldo');
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(8*10);
            readBuffer.rows.should.match(8);
            readBuffer.columns.should.match(10);

            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Waldo', {start: [3,4], stride: [1,1], count: [2,2]});
            readAsBuffer.readDoubleLE(0*8).should.equal(5.0);
            readAsBuffer.readDoubleLE(1*8).should.equal(5.0);
            readAsBuffer.readDoubleLE(2*8).should.equal(1.0);
            readAsBuffer.readDoubleLE(3*8).should.equal(2.0);
            group.close();
        });
        after(function*() {
          file.close();
        });
    });
    describe.skip("varlen char arrays", function() {
        let file;
        before(function*() {
          file = new hdf5.File('./roothaan.h5', Access.ACC_RDWR);
        });
        it("create array of varlen's", function() {
            let group=file.createGroup('pmcservices/Quotes');
            const quotes=new Array(7);
            quotes[0]="Never put off till tomorrow what may be done day after tomorrow just as well.";
            quotes[1]="I have never let my schooling interfere with my education";
            quotes[2]="Reader, suppose you were an idiot. And suppose you were a member of Congress. But I repeat myself.";
            quotes[3]="Substitute 'damn' every time you're inclined to write 'very;' your editor will delete it and the writing will be just as it should be.";
            quotes[4]="Don’t go around saying the world owes you a living. The world owes you nothing. It was here first.";
            quotes[5]="Loyalty to country ALWAYS. Loyalty to government, when it deserves it.";
            quotes[6]="What would men be without women? Scarce, sir...mighty scarce.";
            h5lt.makeDataset(group.id, "Mark Twain", quotes);
            group.close();
            file.close();
            file = new hdf5.File('./roothaan.h5', Access.ACC_RDWR);
            group=file.openGroup('pmcservices/Quotes');
            const array=h5lt.readDataset(group.id, 'Mark Twain');
            group.close();
        });
        after(function*() {
          file.close();
        });
    });

    describe.skip("varlen chars", function() {
        let file;
        before(function*() {
          file = new hdf5.File('./test/examples/nba.h5', Access.ACC_RDWR);
        });
        it("read varlen's", function*() {
            const array=h5lt.readDataset(file.id, 'player');
            console.dir(array.length);
            if(array.constructor.name==='Array'){
                for(let mIndex=0;mIndex<array.length;mIndex++){
                    console.dir(array[mIndex]);
                }
            }
        });
        after(function*() {
          file.close();
        });
    });

});
