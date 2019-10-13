'use strict';
require("should");

const hdf5Lib       = require('..');
const globs         = require('../lib/globals');

const hdf5          = hdf5Lib.hdf5;
const h5tb          = hdf5Lib.h5tb;
const Access        = globs.Access;
const CreationOrder = globs.CreationOrder;
const HLType        = globs.HLType;
const H5Type        = globs.H5Type;

describe("testing table interface ",function(){

    describe("create an h5, group and some tables ",function(){
        // open hdf file
        let file;
        let group;
        before(function(done) {
            file  = new hdf5.File('./h5tb.h5', Access.ACC_TRUNC);
            group = file.createGroup('pmc/refinement');
            done();
        });

        it("should be Table io ", function(done) {
            group.id.should.not.equal(-1);
            const tableModel=new Array(4);
            const fieldArray1=new Uint32Array(5);
            fieldArray1.name="Index";
            fieldArray1[0]=0;
            fieldArray1[1]=1;
            fieldArray1[2]=2;
            fieldArray1[3]=3;
            fieldArray1[4]=4;
            tableModel[0]=fieldArray1;
            const fieldArray2=new Float64Array(5);
            fieldArray2.name="Count Up";
            fieldArray2[0]=1.0;
            fieldArray2[1]=2.0;
            fieldArray2[2]=3.0;
            fieldArray2[3]=4.0;
            fieldArray2[4]=5.0;
            tableModel[1]=fieldArray2;
            const fieldArray3=new Float64Array(5);
            fieldArray3.name="Count Down";
            fieldArray3[0]=5.0;
            fieldArray3[1]=4.0;
            fieldArray3[2]=3.0;
            fieldArray3[3]=2.0;
            fieldArray3[4]=1.0;
            tableModel[2]=fieldArray3;
            const fieldArray4=new Array(5);
            fieldArray4.name="Residues";
            fieldArray4[0]="ALA";
            fieldArray4[1]="VAL";
            fieldArray4[2]="HIS";
            fieldArray4[3]="LEU";
            fieldArray4[4]="HOH";
            tableModel[3]=fieldArray4;
            h5tb.makeTable(group.id, 'Reflections', tableModel);
            done();
        });

        it("should append to Table ", function(done) {
            const tableModel=new Array(4);
            const fieldArray1=new Uint32Array(2);
            fieldArray1.name="Index";
            fieldArray1[0]=5;
            fieldArray1[1]=6;
            tableModel[0]=fieldArray1;
            const fieldArray2=new Float64Array(2);
            fieldArray2.name="Count Up";
            fieldArray2[0]=6.0;
            fieldArray2[1]=7.0;
            tableModel[1]=fieldArray2;
            const fieldArray3=new Float64Array(2);
            fieldArray3.name="Count Down";
            fieldArray3[0]=7.0;
            fieldArray3[1]=6.0;
            tableModel[2]=fieldArray3;
            const fieldArray4=new Array(2);
            fieldArray4.name="Residues";
            fieldArray4[0]="ARG";
            fieldArray4[1]="GLY";
            tableModel[3]=fieldArray4;
            h5tb.appendRecords(group.id, 'Reflections', tableModel);
            const info=h5tb.getTableInfo(group.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            const fieldInfo=h5tb.getFieldInfo(group.id, 'Reflections');
            fieldInfo.length.should.equal(4);
            fieldInfo[3].should.equal("Residues");
            done();
        });

        it("should overwrite records in Table ", function(done) {
            const tableModel=new Array(4);
            const fieldArray1=new Uint32Array(2);
            fieldArray1.name="Index";
            fieldArray1[0]=3;
            fieldArray1[1]=4;
            tableModel[0]=fieldArray1;
            const fieldArray2=new Float64Array(2);
            fieldArray2.name="Count Up";
            fieldArray2[0]=8.0;
            fieldArray2[1]=9.0;
            tableModel[1]=fieldArray2;
            const fieldArray3=new Float64Array(2);
            fieldArray3.name="Count Down";
            fieldArray3[0]=9.0;
            fieldArray3[1]=7.0;
            tableModel[2]=fieldArray3;
            const fieldArray4=new Array(2);
            fieldArray4.name="Residues";
            fieldArray4[0]="ASN";
            fieldArray4[1]="GLN";
            tableModel[3]=fieldArray4;
            h5tb.writeRecords(group.id, 'Reflections', 3, tableModel);
            const info=h5tb.getTableInfo(group.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            done();
        });

        it("should close pmc/refinement ", function(done) {
            group.close();
            done();
        });
    });

    describe("should read & modify table", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('./h5tb.h5', Access.ACC_RDWR);
            done();
        });

        it("should be Table input ", function(done) {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            let table=h5tb.readTable(groupTarget.id, "Reflections");
            table.length.should.equal(4);
            table[0].length.should.equal(7);
            table=h5tb.readRecords(groupTarget.id, "Reflections", 3, 2);
            table.length.should.equal(4);
            table[0].length.should.equal(2);
            groupTarget.close();
            done();
        });

        it("should delete records from Table ", function(done) {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            h5tb.deleteRecord(groupTarget.id, "Reflections", 3, 2);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(5);
            groupTarget.close();
            done();
        });

        it("should insert records into Table ", function(done) {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const tableModel=new Array(4);
            const fieldArray1=new Uint32Array(2);
            fieldArray1.name="Index";
            fieldArray1[0]=3;
            fieldArray1[1]=4;
            tableModel[0]=fieldArray1;
            const fieldArray2=new Float64Array(2);
            fieldArray2.name="Count Up";
            fieldArray2[0]=8.0;
            fieldArray2[1]=9.0;
            tableModel[1]=fieldArray2;
            const fieldArray3=new Float64Array(2);
            fieldArray3.name="Count Down";
            fieldArray3[0]=9.0;
            fieldArray3[1]=7.0;
            tableModel[2]=fieldArray3;
            const fieldArray4=new Array(2);
            fieldArray4.name="Residues";
            fieldArray4[0]="ASN";
            fieldArray4[1]="GLN";
            tableModel[3]=fieldArray4;
            h5tb.insertRecord(groupTarget.id, "Reflections", 3, tableModel);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            groupTarget.close();
            done();
        });

        it("should overwrite fields in Table ", function(done) {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const tableModel=new Array(2);
            const fieldArray1=new Uint32Array(7);
            fieldArray1.name="Index";
            fieldArray1[0]=0;
            fieldArray1[1]=1;
            fieldArray1[2]=2;
            fieldArray1[3]=3;
            fieldArray1[4]=4;
            fieldArray1[5]=5;
            fieldArray1[6]=6;
            tableModel[0]=fieldArray1;
            const fieldArray2=new Float64Array(7);
            fieldArray2.name="Count Down";
            fieldArray2[0]=7.0;
            fieldArray2[1]=6.0;
            fieldArray2[2]=5.0;
            fieldArray2[3]=4.0;
            fieldArray2[4]=3.0;
            fieldArray2[5]=2.0;
            fieldArray2[6]=1.0;
            tableModel[1]=fieldArray2;
            h5tb.writeFieldsName(groupTarget.id, "Reflections", 0, tableModel);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            groupTarget.close();
            done();
        });

        it("should overwrite fields in Table by index ", function(done) {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const tableModel=new Array(2);
            const fieldArray1=new Uint32Array(7);
            fieldArray1[0]=0;
            fieldArray1[1]=1;
            fieldArray1[2]=2;
            fieldArray1[3]=3;
            fieldArray1[4]=4;
            fieldArray1[5]=5;
            fieldArray1[6]=6;
            tableModel[0]=fieldArray1;
            const fieldArray2=new Float64Array(7);
            fieldArray2[0]=1.0;
            fieldArray2[1]=2.0;
            fieldArray2[2]=3.0;
            fieldArray2[3]=4.0;
            fieldArray2[4]=5.0;
            fieldArray2[5]=6.0;
            fieldArray2[6]=7.0;
            tableModel[1]=fieldArray2;
            h5tb.writeFieldsIndex(groupTarget.id, "Reflections", 0, tableModel, [0, 1]);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            groupTarget.close();
            done();
        });

        it("should read fields in Table", function(done) {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const table=h5tb.readFieldsName(groupTarget.id, "Reflections", 1, 5, ["Index", "Residues"]);
            table[1][0].should.equal("VAL");
            table[1][1].should.equal("HIS");
            table[1][2].should.equal("ASN");
            table[1][3].should.equal("GLN");
            table[1][4].should.equal("ARG");
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            groupTarget.close();
            done();
        });

        it("should read fields in Table by index", function(done) {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const table=h5tb.readFieldsIndex(groupTarget.id, "Reflections", 0, 7, [0, 2]);
            table[1][0].should.equal(7.0);
            table[1][1].should.equal(6.0);
            table[1][2].should.equal(5.0);
            table[1][3].should.equal(4.0);
            table[1][4].should.equal(3.0);
            table[1][5].should.equal(2.0);
            table[1][6].should.equal(1.0);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            groupTarget.close();
            done();
        });

        it("should delete field in Table", function(done) {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            h5tb.deleteField(groupTarget.id, "Reflections", "Index");
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(3);
            info.nrecords.should.equal(7);
            groupTarget.close();
            done();
        });

        it("should insert field in Table", function(done) {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const tableModel=new Array(1);
            const fieldArray1=new Uint32Array(7);
            fieldArray1.name="Indices";
            fieldArray1[0]=0;
            fieldArray1[1]=1;
            fieldArray1[2]=2;
            fieldArray1[3]=3;
            fieldArray1[4]=4;
            fieldArray1[5]=5;
            fieldArray1[6]=6;
            tableModel[0]=fieldArray1;
            h5tb.insertField(groupTarget.id, "Reflections", 0, tableModel);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            groupTarget.flush();
            groupTarget.close();
            done();
        });

        it("should add records from Table", function(done) {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const tableModel=new Array(4);
            const fieldArray1=new Uint32Array(2);
            fieldArray1.name="Indices";
            fieldArray1[0]=0;
            fieldArray1[1]=1;
            tableModel[0]=fieldArray1;
            const fieldArray2=new Float64Array(2);
            fieldArray2.name="Count Up";
            fieldArray2[0]=1.0;
            fieldArray2[1]=2.0;
            tableModel[1]=fieldArray2;
            const fieldArray3=new Float64Array(2);
            fieldArray3.name="Count Down";
            fieldArray3[0]=5.0;
            fieldArray3[1]=4.0;
            tableModel[2]=fieldArray3;
            const fieldArray4=new Array(2);
            fieldArray4.name="Residues";
            fieldArray4[0]="ALA";
            fieldArray4[1]="VAL";
            tableModel[3]=fieldArray4;
            h5tb.makeTable(groupTarget.id, 'More Reflections', tableModel);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            h5tb.addRecordsFrom(groupTarget.id, "Reflections", 0, info.nrecords, "More Reflections", 2);
            const info2=h5tb.getTableInfo(groupTarget.id, 'More Reflections');
            info2.nfields.should.equal(4);
            info2.nrecords.should.equal(9);
            groupTarget.close();
            done();
        });

        it("should combine Tables", function(done) {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            h5tb.combineTables(groupTarget.id, "Reflections", groupTarget.id, "More Reflections", "Stack");
            const info2=h5tb.getTableInfo(groupTarget.id, 'Stack');
            info2.nfields.should.equal(4);
            info2.nrecords.should.equal(16);
            groupTarget.close();
            done();
        });

        after(function(done) {
            file.close();
            done();
        });
    });

    describe("should align table", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('./h5tbAlignment.h5', Access.ACC_TRUNC);
            done();
        });

        it("should be Table aligned ", function(done) {
            const table = new Array();

            const tempModeCol = new Int8Array(1);
            tempModeCol.name = 'temperatureMode';
            tempModeCol[0] = -1;
            table.push(tempModeCol);

            const tempSollCol = new Float64Array(1);
            tempSollCol.name = 'temperatureSoll';
            tempSollCol[0] =-1;
            table.push(tempSollCol);

            h5tb.makeTable(file.id, 'infos', table);
            done();
        });

        it("should write Table long long ", function(done) {
            const table = new Array();

            const idCol = new Array();
            idCol.name = 'id';
            idCol.type = H5Type.H5T_NATIVE_LLONG;
            idCol[0] = 12;
            table.push(idCol);

            const timestampCol = new Array();
            timestampCol.name = 'timestamp';
            timestampCol.type = H5Type.H5T_NATIVE_ULLONG;
            timestampCol[0] = 5534023222126287257;
            table.push(timestampCol);

            const tempModeCol= new Int8Array(1);
            tempModeCol.name = 'mode';
            tempModeCol[0] = 0;
            table.push(tempModeCol);

            try{
              h5tb.makeTable(file.id, 'infos2', table);
                try{
                  let readTable=h5tb.readTable(file.id, "infos2");
                  try{
                    readTable.length.should.equal(3);
                    //readTable[0].length.should.equal(1);
                  }
                  catch (e) {
                      console.log("\t\t"+e.message);
                  }
                }
                catch (e) {
                  console.log("\t\t"+e.message);
                  e.message.should.equal("unsupported data type");
                }
            }
            catch (e) {
                console.log("\t"+e.message);
                e.message.should.equal("unsupported data type");
            }
            done();
        });

        after(function(done) {
            file.close();
            done();
        });
    });

    describe("write and read all datatypes", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('./h5tbDatatypes.h5', Access.ACC_TRUNC);
            done();
        });

        it("should write and read table", function(done) {
            // Write Table

            const table = new Array();

            const count = 2

            const types = [new Int8Array(count),new Int16Array(count),new Int32Array(count),new Float32Array(count), new Float64Array(count)];

            for (let i = 0; i < types.length; i++) {                   
                const col = types[i];
                col.name = 'col'+i;

                for (let c = 0; c < count; c++) {
                    col[c] = ((i+10)*(c+2));              
                }
                
                table.push(col);
            }

            h5tb.makeTable(file.id, 'infos', table);

            // Read Table
            const readTable=h5tb.readTable(file.id, "infos");
            readTable.length.should.equal(types.length);

            for (let i = 0; i < readTable.length; i++) {                   
                const col = readTable[i];
                col.name.should.equal('col'+i);

                for (let c = 0; c < count; c++) {
                    col[c].should.equal(((i+10)*(c+2)));              
                }
                
            }

            done();
        });

        after(function(done) {
            file.close();
            done();
        });
    });

    describe.skip("Reading a large test case", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('/home/roger/Downloads/MillionSongSubset/data/A/A/A/TRAAAAW128F429D538.h5', Access.ACC_RDONLY);
          done();
        });
        let groupTarget;
        it("loop thru", function(done) {
            console.log(file.getNumObjs());
            var memberNames=file.getMemberNamesByCreationOrder();
            try {
                for(var mn in memberNames){
                   console.log(memberNames[mn]);
                   var childType=file.getChildType(memberNames[mn]);
                   switch(childType){
                       case H5OType.H5O_TYPE_GROUP:
                       var group=file.openGroup(memberNames[mn]);
                       var groupMemberNames=group.getMemberNamesByCreationOrder();
                       for(var gmn in groupMemberNames){
                          console.log("\t"+groupMemberNames[gmn]);
                       }
                       group.close();
                       break;

                       case H5OType.H5O_TYPE_DATASET:

                   var dsType=file.getDatasetType(memberNames[mn]);
                   console.log("dsType "+dsType);
                   switch(dsType){
                     case HLType.HL_TYPE_LITE:
                       var ds=h5lt.readDataset(file.id, memberNames[mn]);
                   console.log("rank: "+ds.rank);
                       break;


                   }
                   break;
                   }
                }
                 var group=file.openGroup("metadata");
                 var childType=group.getChildType("songs");
                 var dsType=group.getDatasetType("songs");
                 console.log("type "+dsType+" "+HLType.HL_TYPE_TABLE);
                 var tableInfo=h5tb.getTableInfo(group.id, "songs");
                 console.log("tableInfo "+tableInfo.nfields+" "+tableInfo.nrecords);
                 var readTable=h5tb.readTable(group.id, "songs");
                 for(var fieldIndex in readTable){
                   console.log(readTable[fieldIndex].name);
                   console.log(readTable[fieldIndex][0]);
                 }

                 //console.log("readTable "+readTable);
                 group.close();
            } catch (e) {
                console.log(e.message);
            }
            done();
        });
        
        it.skip("visit thru", function(done) {
            groupTarget=file.openGroup('pmcservices/sodium-icosanoate/Documents', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.id.should.not.equal(-1);
            try {
                var count=0;
                file.visit(1, function(r, xpath) {
                    //console.dir("visiting name: "+xpath);
                    count++;
                });
                console.log("cout "+count);
                count.should.equal(204);
            } catch (e) {
                console.log(e.message);
            }
            groupTarget.close();
            done();
        });
        
        after(function(done) {
          file.close();
          done();
        });
    });

});

