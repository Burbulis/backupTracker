

select fileGuid,hashId,idx from buckets_and_blocks bab
 inner join blocks b on b.blockHash = bab.blockHash 
where fileGuid like "%21dee8b"

select blocks.chk,blocks.layerHash,buckets_and_blocks.idx,blocks.size from buckets_and_blocks  inner join blocks on blocks.blockHash = buckets_and_blocks.blockHash and blocks.layerHash= buckets_and_blocks.hashId
where  buckets_and_blocks.fileGuid like '%ad282' and blocks.layerHash = 76702598729822 and blocks.chk > 0;

select SUM(blocks.size) from buckets_and_blocks  inner join blocks on blocks.blockHash = buckets_and_blocks.blockHash 
where  buckets_and_blocks.fileGuid like '%21dee8b' and blocks.layerHash = 76702598729822 and buckets_and_blocks.hashId = 76702598729822 and blocks.chk > 0;