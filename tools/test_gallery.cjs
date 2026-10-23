// Test navigation/download/compare/zoom wiring without pretending this is a browser visual audit.
const fs=require('fs'),vm=require('vm'),path=require('path'),assert=require('assert');
class Element {
    constructor(){this.children=[];this.dataset={};this.attributes={};this.classList={values:new Set(),add(v){this.values.add(v)},remove(v){this.values.delete(v)},contains(v){return this.values.has(v)}};}
    append(c){this.children.push(c)} setAttribute(k,v){this.attributes[k]=String(v)} focus(){} scrollIntoView(){}
}
const elements=new Map();const element=id=>{if(!elements.has(id))elements.set(id,new Element());return elements.get(id)};
const handlers={};const document={getElementById:element,createElement:()=>new Element(),addEventListener:(name,fn)=>handlers[name]=fn};
const file=fs.readFileSync(path.join(__dirname,'../Design/UI-Previews/index.html'),'utf8');
const script=file.match(/<script>([\s\S]*?)<\/script>/)[1];
vm.runInNewContext(script,{document,location:{hash:''},matchMedia:()=>({matches:true})});
assert.equal(element('nav').children.length,10);assert.equal(element('grid').children.length,10);
for(const button of element('nav').children){button.onclick();const name=button.dataset.name;assert.equal(element('hero').src,name+'.png');assert.equal(element('png').href,name+'.png');assert.equal(element('svg').href,name+'.svg');assert.equal(button.attributes['aria-pressed'],'true');}
element('compare').onclick();assert(element('hero').src.endsWith('Bundle-rack-reference.png'));
element('compare').onclick();assert.equal(element('hero').src,'DFX.png');
element('hero').onclick();assert(element('lightbox').classList.contains('open'));handlers.keydown({key:'Escape'});assert(!element('lightbox').classList.contains('open'));
console.log('PASS gallery: all ten selections, download links, reference comparison and zoom/Escape');

element('advanced').onclick();assert.equal(element('hero').src,'DFX-Advanced.png');assert.equal(element('png').href,'DFX-Advanced.png');element('advanced').onclick();assert.equal(element('hero').src,'DFX.png');
